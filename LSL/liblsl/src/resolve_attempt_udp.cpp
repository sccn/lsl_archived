#include <iostream>
#include <istream>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/functional/hash.hpp>
#include "api_config.h"
#include "resolve_attempt_udp.h"
#include "resolver_impl.h"
#include "socket_utils.h"


// === implementation of the resolver_burst_udp class ===

using namespace lsl;
using namespace boost::asio;
using boost::posix_time::millisec;

/**
* Instantiate and set up a new resolve attempt.
* @param io The io_service that will run the async operations.
* @param protocol The protocol (either udp::v4() or udp::v6()) to use for communications; 
*				  only the subset of target addresses matching this protocol will be considered.
* @param targets A list of udp::endpoint that should be targetd by this query.
* @param query The query string to send (usually a set of conditions on the properties of the stream info that should be searched,
*              for example "name='BioSemi' and type='EEG'" (without the outer ""). See stream_info_impl::matches_query() for the 
*			   definition of a query.
* @param results Reference to a container into which results are stored; potentially shared with other parallel resolve operations.
*			     Since this is not thread-safe all operations modifying this must run on the same single-threaded IO service.
* @param results_mut Reference to a mutex that protects the container.
* @param cancel_after The time duration after which the attempt is automatically cancelled, i.e. the receives are ended.
* @param registry A registry where the attempt can register itself as active so it can be cancelled during shutdown.
*/
resolve_attempt_udp::resolve_attempt_udp(io_service &io, const udp &protocol, const std::vector<udp::endpoint> &targets, const std::string &query, result_container &results, boost::mutex &results_mut, double cancel_after, cancellable_registry *registry): 
	io_(io), results_(results), results_mut_(results_mut), cancel_after_(cancel_after), cancelled_(false), is_v4_(protocol == udp::v4()), protocol_(protocol),
	targets_(targets), unicast_socket_(io), broadcast_socket_(io), multicast_socket_(io), recv_socket_(io), query_(query), cancel_timer_(io)
{
	// open the sockets that we might need
	recv_socket_.open(protocol);
	try {
		bind_port_in_range(recv_socket_,protocol);
	} catch(std::exception &e) {
		std::cerr << "Could not bind to a port in the configured port range; using a randomly assigned one: " << e.what() << std::endl;
	}
	unicast_socket_.open(protocol);
	try {
		broadcast_socket_.open(protocol);
		broadcast_socket_.set_option(socket_base::broadcast(true));
	} catch(std::exception &e) {
		std::cerr << "Cannot open UDP broadcast socket for resolves: " << e.what() << std::endl;
	}
	try {
		multicast_socket_.open(protocol);
		multicast_socket_.set_option(ip::multicast::hops(api_config::get_instance()->multicast_ttl()));
	} catch(std::exception &e) {
		std::cerr << "Cannot open UDP multicast socket for resolves: " << e.what() << std::endl;
	}

	// precalc the query id (hash of the query string, as string)
	query_id_ = boost::lexical_cast<std::string>(boost::hash<std::string>()(query));
	// precalc the query message
	std::ostringstream os; os.precision(16);
	os << "LSL:shortinfo\r\n";
	os << query_ << "\r\n";
	os << recv_socket_.local_endpoint().port() << " " << query_id_ << "\r\n";
	query_msg_ = os.str();

	// register ourselves as a candidate for cancellation
	if (registry)
		register_at(registry);
}

resolve_attempt_udp::~resolve_attempt_udp() {
	// make sure that the cancel is unregistered before the resolve attempt is being deleted...
	unregister_from_all();
}

// === externally-triggered asynchronous commands ===

/// Start the attempt.
void resolve_attempt_udp::begin() {
	// initiate the result gathering chain
	receive_next_result();
	// initiate the send chain
	send_next_query(targets_.begin());

	// also initiate the cancel event, if desired
	if (cancel_after_ != FOREVER) {
		cancel_timer_.expires_from_now(millisec(1000*cancel_after_));
		cancel_timer_.async_wait(boost::bind(&resolve_attempt_udp::handle_timeout,shared_from_this(),placeholders::error));
	}
}

/// Post a command to cancel all operations.
void resolve_attempt_udp::cancel() {
	io_.post(boost::bind(&resolve_attempt_udp::do_cancel,shared_from_this()));
}


// === receive loop ===

/// This function asks to receive the next result packet
void resolve_attempt_udp::receive_next_result() {
	recv_socket_.async_receive_from(buffer(resultbuf_),remote_endpoint_,
		boost::bind(&resolve_attempt_udp::handle_receive_outcome,shared_from_this(),placeholders::error,placeholders::bytes_transferred));
}

/// Handler that gets called when a receive has completed
void resolve_attempt_udp::handle_receive_outcome(error_code err, std::size_t len) {
	if (!cancelled_ && err != error::operation_aborted && err != error::not_connected && err != error::not_socket) {
		if (!err) {
			try {
				// first parse & check the query id
				std::istringstream is(std::string(resultbuf_,len));
				std::string returned_id; getline(is,returned_id); boost::trim(returned_id);
				if (returned_id == query_id_) {
					// parse the rest of the query into a stream_info
					stream_info_impl info;
					std::ostringstream os; os << is.rdbuf();
					info.from_shortinfo_message(os.str());
					std::string uid = info.uid();
					{
						// update the results
						boost::lock_guard<boost::mutex> lock(results_mut_);
						if (results_.find(uid) == results_.end())
							results_[uid] = std::make_pair(info,lsl_clock()); // insert new result
						else							
							results_[uid].second = lsl_clock(); // update only the receive time
						// ... also update the address associated with the result (but don't override the 
						// address of an earlier record for this stream since this would be the faster route)
						if (remote_endpoint_.address().is_v4()) {
							if (results_[uid].first.v4address().empty())
								results_[uid].first.v4address(remote_endpoint_.address().to_string());
						} else {
							if (results_[uid].first.v6address().empty())
								results_[uid].first.v6address(remote_endpoint_.address().to_string());
						}
					}
				}
			} catch(std::exception &e) {
				std::cerr << "resolve_attempt_udp: hiccup while processing the received data: " << e.what() << std::endl;
			}
		}
		// ask for the next result
		receive_next_result();
	}
}


// === send loop ===

/// Thus function starts an async send operation for the given current endpoint
void resolve_attempt_udp::send_next_query(endpoint_list::const_iterator i) {
	if (i != targets_.end() && !cancelled_) {
		udp::endpoint ep(*i);
		// endpoint matches our active protocol?
		if (ep.address().is_v4() == is_v4_) {
			// select socket to use
			udp::socket &sock = (ep.address().to_string() == "255.255.255.255") ? broadcast_socket_ : (ep.address().is_multicast() ? multicast_socket_ : unicast_socket_);
			// and send the query over it
			sock.async_send_to(boost::asio::buffer(query_msg_), ep,
				boost::bind(&resolve_attempt_udp::handle_send_outcome,shared_from_this(),++i,placeholders::error));
		} else
			// otherwise just go directly to the next query
			send_next_query(++i);
	}
}

/// Handler that gets called when a send has completed
void resolve_attempt_udp::handle_send_outcome(endpoint_list::const_iterator i, error_code err) {
	if (!cancelled_ && err != error::operation_aborted && err != error::not_connected && err != error::not_socket)
		send_next_query(i);
}


// === cancellation logic ===

/// Give up the current operations.
void resolve_attempt_udp::handle_timeout(error_code err) {
	if (!err)
		do_cancel();
}

/// Cancel the outstanding operations.
void resolve_attempt_udp::do_cancel() {
	try {
		cancelled_ = true;
		if (unicast_socket_.is_open())
			unicast_socket_.close();
		if (multicast_socket_.is_open())
			multicast_socket_.close();
		if (broadcast_socket_.is_open())
			broadcast_socket_.close();
		if (recv_socket_.is_open())
			recv_socket_.close();
		cancel_timer_.cancel();
	} catch(std::exception &e) {
		std::cerr << "Unexpected error while trying to cancel operations of resolve_attempt_udp: " << e.what() << std::endl;
	}
}

