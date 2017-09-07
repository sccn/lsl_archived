#include <iostream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/container/flat_set.hpp>
#include "tcp_server.h"
#include "socket_utils.h"


// === implementation of the tcp_server class ===

using namespace lsl;
using namespace boost::asio;

/**
* Construct a new TCP server for a stream outlet.
* This opens a new TCP server port (in the allowed range) and, if successful,
* updates the stream_info object with the data of this connection. To have it serve
* connection requests, the member function begin_serving() must be called once.
* The latter should ideally not be done before the UDP service port has been
* successfully initialized, as well.
* @param info A stream_info that is shared with other server objects.
* @param io An io_service that is shared with other server objects.
* @param sendbuf A send buffer that is shared with other server objects.
* @param protocol The protocol (IPv4 or IPv6) that shall be serviced by this server.
* @param chunk_size The preferred chunk size, in samples. If 0, the pushthrough flag determines the effective chunking.
*/
tcp_server::tcp_server(const stream_info_impl_p &info, const io_service_p &io, const send_buffer_p &sendbuf, const sample::factory_p &factory, tcp protocol, int chunk_size): chunk_size_(chunk_size), shutdown_(false), info_(info), io_(io), factory_(factory), send_buffer_(sendbuf), acceptor_(new tcp::acceptor(*io)) {
	// open the server connection
	acceptor_->open(protocol);

	// bind to and listen on a free port
	int port = bind_and_listen_to_port_in_range(*acceptor_,protocol,10);

	// and assign connection-dependent fields
	// (note: this may be assigned multiple times by multiple TCPs during setup but does not matter)
	info_->session_id(api_config::get_instance()->session_id());
	info_->uid(boost::uuids::to_string(boost::uuids::random_generator()()));
	info_->created_at(lsl_clock());
	info_->hostname(ip::host_name());
    if (protocol == tcp::v4())
		info_->v4data_port(port);
	else
		info_->v6data_port(port);
}


// === externally issued asynchronous commands ===

/**
* Begin serving TCP connections.
* Should not be called before info has been fully initialized by all involved parties (tcp_server, udp_server...)
*/
void tcp_server::begin_serving() {
	// pre-generate the info's messages
	shortinfo_msg_ = info_->to_shortinfo_message();
	fullinfo_msg_ = info_->to_fullinfo_message();
	// start accepting connections
	accept_next_connection();
}

/**
* Initiate teardown of IO processes.
* The actual teardown will be performed by the IO thread that runs the operations of this server.
*/
void tcp_server::end_serving() {
	// the shutdown flag informs the transfer thread that we're shutting down
	shutdown_ = true;
	// issue closure of the server socket; this will result in a cancellation of the associated IO operations
	io_->post(boost::bind(&tcp::acceptor::close,acceptor_));
	// issue closure of all active client session sockets; cancels the related outstanding IO jobs
	close_inflight_sockets();
	// also notify any transfer threads that are blocked waiting for a sample by sending them one (= a ping)
	send_buffer_->push_sample(factory_->new_sample(lsl_clock(), true));
}


// === accept loop ===

/// Start accepting a new connection.
void tcp_server::accept_next_connection() {
	try {
		// make a new session
		client_session_p newsession(new client_session(shared_from_this()));
		// accept a connection on the session's socket
		acceptor_->async_accept(*newsession->socket(),
			boost::bind(&tcp_server::handle_accept_outcome,shared_from_this(),newsession,placeholders::error));
	}  catch(std::exception &e) {
		std::cerr << "Error during tcp_server::accept_next_connection (id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
	}
}

/// Handler that is called when the accept has finished.
void tcp_server::handle_accept_outcome(client_session_p newsession, error_code err) {
	if (err != error::operation_aborted && err != error::shut_down && !shutdown_) {
		if (!err)
			// no error: start processing the new connection
			newsession->begin_processing();
		// and move on to the next connection
		accept_next_connection();
	}
}


// === graceful cancellation of in-flight sockets ===

/// Register an in-flight (active) session socket with the server (so that we can close it when a shutdown is requested externally).
void tcp_server::register_inflight_socket(const tcp_socket_p &sock) {
	boost::lock_guard<boost::recursive_mutex> lock(inflight_mut_);
	inflight_.insert(sock);
}

/// Unregister an in-flight session socket.
void tcp_server::unregister_inflight_socket(const tcp_socket_p &sock) {
	boost::lock_guard<boost::recursive_mutex> lock(inflight_mut_);
	inflight_.erase(sock);
}

/// Post a close of all in-flight sockets.
void tcp_server::close_inflight_sockets() {
	boost::lock_guard<boost::recursive_mutex> lock(inflight_mut_);
	for (std::set<tcp_socket_p>::iterator i=inflight_.begin(); i!=inflight_.end(); i++)
		io_->post(boost::bind(&shutdown_and_close<tcp_socket_p,tcp>,*i));
}



//
// === implementation of the tcp_server::client_session class ===
//


/// Instantiate a new session & its socket.
tcp_server::client_session::client_session(const tcp_server_p &serv): registered_(false), io_(serv->io_), serv_(serv), sock_(tcp_socket_p(new tcp::socket(*serv->io_))), requeststream_(&requestbuf_), use_byte_order_(0), data_protocol_version_(100) {	}

/**
* Destructor. Unregisters the socket from the server & closes it.
* Note: This will only be called after the transfer thread and any other operations are done, since these ops hold shared ptrs to this object.
*/
tcp_server::client_session::~client_session() {
	try {
		if (registered_)
			serv_->unregister_inflight_socket(sock_);
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error in client_session destructor (id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "Severe error during client session shutdown." << std::endl;
	}
}

/// Get the socket of this session.
tcp_socket_p tcp_server::client_session::socket() { return sock_; }

/// Begin processing this session.
void tcp_server::client_session::begin_processing() {
	try {
		sock_->set_option(boost::asio::ip::tcp::no_delay(true));
		// register this socket as "in-flight" with the server (so that any subsequent ops on it can be aborted if necessary)
		serv_->register_inflight_socket(sock_);
		registered_ = true;
		// read the request line
		async_read_until(*sock_, requestbuf_, "\r\n",
			boost::bind(&client_session::handle_read_command_outcome,shared_from_this(),placeholders::error));
	} catch(std::exception &e) {
		std::cerr << "Error during client_session::begin_processing (id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
	}
}

/// Handler that gets called when the reading of the 1st line (request line) of the inbound message finished.
void tcp_server::client_session::handle_read_command_outcome(error_code err) {
	try {
		if (!err) {
			// parse request method
			std::string method; getline(requeststream_,method); boost::trim(method);
			if (method == "LSL:shortinfo")
				// shortinfo request: read the content query string
				async_read_until(*sock_, requestbuf_, "\r\n",
					boost::bind(&client_session::handle_read_query_outcome,shared_from_this(),placeholders::error));
			if (method == "LSL:fullinfo")
				// fullinfo request: reply right away
				async_write(*sock_, boost::asio::buffer(serv_->fullinfo_msg_),
					boost::bind(&client_session::handle_send_outcome,shared_from_this(),placeholders::error));
			if (method == "LSL:streamfeed")
				// streamfeed request (1.00): read feed parameters
				async_read_until(*sock_, requestbuf_, "\r\n",
					boost::bind(&client_session::handle_read_feedparams,shared_from_this(),100,"",placeholders::error));
			if (boost::algorithm::starts_with(method,"LSL:streamfeed/")) {
				// streamfeed request with version: read feed parameters
				std::vector<std::string> parts; boost::algorithm::split(parts,method,boost::algorithm::is_any_of(" \t"));
				int request_protocol_version = boost::lexical_cast<int>(parts[0].substr(parts[0].find_first_of("/")+1));
				std::string request_uid = (parts.size()>1) ? parts[1] : "";
				async_read_until(*sock_, requestbuf_, "\r\n\r\n",
					boost::bind(&client_session::handle_read_feedparams,shared_from_this(),request_protocol_version,request_uid,placeholders::error));
			}
		}
	} catch(std::exception &e) {
		std::cerr << "Unexpected error while parsing a client command (id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
	}
}

/// Handler that gets called after finishing reading of the query line.
void tcp_server::client_session::handle_read_query_outcome(error_code err) {
	try {
		if (!err) {
			// read the query line
			std::string query; getline(requeststream_,query); boost::trim(query);
			if (serv_->info_->matches_query(query))
				// matches: reply (otherwise just close the stream)
				async_write(*sock_, boost::asio::buffer(serv_->shortinfo_msg_),
					boost::bind(&client_session::handle_send_outcome,shared_from_this(),placeholders::error));
		}
	} catch(std::exception &e) {
		std::cerr << "Unexpected error while parsing a client request (id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
	}
}

/// Handler that gets called after finishing the sending of a reply (nothing to do here).
void tcp_server::client_session::handle_send_outcome(error_code err) { }

/// Helper function to send a status message to the connected party
void tcp_server::client_session::send_status_message(const std::string &str) {
	string_p msg(new std::string(str));
	async_write(*sock_, boost::asio::buffer(*msg),
		boost::bind(&client_session::handle_status_outcome,shared_from_this(),msg,placeholders::error));
}

/// Handler that gets called after finishing the sending of a message, holding a reference to the message.
void tcp_server::client_session::handle_status_outcome(string_p msg, error_code err) { }

/// Handler that gets called after finishing the reading of feedparameters
void tcp_server::client_session::handle_read_feedparams(int request_protocol_version, std::string request_uid, error_code err) {
	try {
		if (!err) {
			// --- protocol negotiation ---
			using namespace boost::algorithm;

			// check request validity
			if (request_protocol_version/100 > api_config::get_instance()->use_protocol_version()/100) {
				send_status_message((boost::format("LSL/%1% 505 Version not supported") % api_config::get_instance()->use_protocol_version()).str());
				return;
			}
			if (!request_uid.empty() && request_uid != serv_->info_->uid()) {
				send_status_message((boost::format("LSL/%1% 404 Not found") % api_config::get_instance()->use_protocol_version()).str());
				return;
			}

			if (request_protocol_version >= 110) {
				int client_byte_order = 1234;			// assume little endian
				double client_endian_performance = 0;	// the other party's endian conversion performance
				bool client_has_ieee754_floats = true;	// the client has IEEE-754 compliant floating point formats
				bool client_supports_subnormals = true;	// the client supports subnormal numbers
				int client_protocol_version = request_protocol_version;	// assume that the client wants to use the same version for data transmission
				int client_value_size = serv_->info_->channel_bytes();	// assume that the client has a standard size for the relevant data type
				channel_format_t format = serv_->info_->channel_format();

				// read feed parameters
				char buf[16384] = {0};
				while (requeststream_.getline(buf,sizeof(buf)) && (buf[0] != '\r')) {
					std::string hdrline(buf);
					int colon = hdrline.find_first_of(":");
					if (colon != std::string::npos) {
						// extract key & value
						std::string type = to_lower_copy(trim_copy(hdrline.substr(0,colon))), rest = to_lower_copy(trim_copy(hdrline.substr(colon+1)));
						// strip off comments
						int semicolon = rest.find_first_of(";");
						if (semicolon != std::string::npos)
							rest = rest.substr(0,semicolon);
						// get the header information
						if (type == "native-byte-order")
							client_byte_order = boost::lexical_cast<int>(rest);
						if (type == "endian-performance")
							client_endian_performance = boost::lexical_cast<double>(rest);
						if (type == "has-ieee754-floats")
							client_has_ieee754_floats = boost::lexical_cast<bool>(rest);
						if (type == "supports-subnormals")
							client_supports_subnormals = boost::lexical_cast<bool>(rest);
						if (type == "value-size")
							client_value_size = boost::lexical_cast<int>(rest);
						if (type == "max-buffer-length")
							max_buffered_ = boost::lexical_cast<int>(rest);
						if (type == "max-chunk-length")
							chunk_granularity_ = boost::lexical_cast<int>(rest);
						if (type == "protocol-version")
							client_protocol_version = boost::lexical_cast<int>(rest);
					}
				}

				// determine the parameters for data transmission
				bool client_suppress_subnormals = false;
				// use least common denominator data protocol version
				data_protocol_version_ = std::min(api_config::get_instance()->use_protocol_version(),client_protocol_version);
				// downgrade to 1.00 (portable binary format) if an unsupported binary conversion is involved
				if (serv_->info_->channel_bytes() != client_value_size)
					data_protocol_version_ = 100;
				if (!format_ieee754[cf_double64] || (format==cf_float32 && !format_ieee754[cf_float32]) || !client_has_ieee754_floats)
					data_protocol_version_ = 100;
				if (data_protocol_version_ >= 110) {
					// decide on the byte order if conflicting
					if (BOOST_BYTE_ORDER != client_byte_order) {
						if (client_byte_order == 2134 && client_value_size>=8) {
							// since we have no implementation for this byte order conversion let the client do it
							use_byte_order_ = BOOST_BYTE_ORDER;
						} else {
							// let the faster party perform the endian conversion
							use_byte_order_ = (client_value_size<=1 || (measure_endian_performance()>client_endian_performance)) ? client_byte_order : BOOST_BYTE_ORDER;
						}
					} else
						use_byte_order_ = BOOST_BYTE_ORDER;
					// determine if subnormal suppression needs to be enabled
					client_suppress_subnormals = (format_subnormal[format] && !client_supports_subnormals);
				}

				// send the response
				std::ostream response_stream(&feedbuf_);
				response_stream << "LSL/" << api_config::get_instance()->use_protocol_version() << " 200 OK\r\n";
				response_stream << "UID: " << serv_->info_->uid() << "\r\n";
				response_stream << "Byte-Order: " << use_byte_order_ << "\r\n";
				response_stream << "Suppress-Subnormals: " << client_suppress_subnormals << "\r\n";
				response_stream << "Data-Protocol-Version: " << data_protocol_version_ << "\r\n";
				response_stream << "\r\n" << std::flush;
			} else {
				// read feed parameters
				requeststream_ >> max_buffered_ >> chunk_granularity_;
			}

			// --- validation ---
			if (data_protocol_version_ == 100) {
				// create a portable output archive to write to
				outarch_.reset(new eos::portable_oarchive(feedbuf_));
				// serialize the shortinfo message into an archive
				*outarch_ << serv_->shortinfo_msg_;
			} else {
				// allocate scratchpad memory for endian conversion, etc.
				scratch_.reset(new char[format_sizes[serv_->info_->channel_format()]*serv_->info_->channel_count()]);
			}

			// send test pattern samples
			boost::scoped_ptr<sample> temp(sample::factory::new_sample_unmanaged(serv_->info_->channel_format(),serv_->info_->channel_count(),0.0,false));
			temp->assign_test_pattern(4); if (data_protocol_version_ >= 110) temp->save_streambuf(feedbuf_,data_protocol_version_,use_byte_order_,scratch_.get()); else *outarch_ << *temp;
			temp->assign_test_pattern(2); if (data_protocol_version_ >= 110) temp->save_streambuf(feedbuf_,data_protocol_version_,use_byte_order_,scratch_.get()); else *outarch_ << *temp;
			// send off the newly created feedheader
			async_write(*sock_,feedbuf_.data(),
				boost::bind(&client_session::handle_send_feedheader_outcome,shared_from_this(),placeholders::error,placeholders::bytes_transferred));
		}
	} catch(std::exception &e) {
		std::cerr << "Unexpected error while serializing the feed header (id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
	}
}

/// Handler that gets called sending the feedheader has completed.
void tcp_server::client_session::handle_send_feedheader_outcome(error_code err, std::size_t n) {
	try {
		if (!err) {
			feedbuf_.consume(n);
			// register outstanding work at the server (will be unregistered at session destruction)
			work_.reset(new io_service::work(*serv_->io_));
			// spawn a sample transfer thread
			boost::thread(&client_session::transfer_samples_thread,this,shared_from_this());
		}
	} catch(std::exception &e) {
		std::cerr << "Unexpected error while handling the feedheader send outcome (id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
	}
}

/// Transfers samples from the server's send buffer into the async send queues of the IO threads.
void tcp_server::client_session::transfer_samples_thread(client_session_p) {
	if (max_buffered_ <= 0)
		return;
	try {
		// make a new consumer queue
		consumer_queue_p queue = serv_->send_buffer_->new_consumer(max_buffered_);
		// the sequence # is merely used to determine chunk boundaries (no need for int64)
		unsigned seqn = 0;
		while (!serv_->shutdown_) {
			try {
				// get next sample from the sample queue (blocking)
				sample_p samp(queue->pop_sample());
				if (serv_->shutdown_)
					break;
				// ignore blank samples (they are basically wakeup notifiers from someone's end_serving())
				if (!samp)
					continue;
				// optionally override the pushthrough flag by the chunk size of the receiver (if set) or of the sender (if set)
				if (chunk_granularity_)
					samp->pushthrough = (((++seqn)%(unsigned)chunk_granularity_) == 0);
				else
					if (serv_->chunk_size_)
						samp->pushthrough = (((++seqn)%(unsigned)serv_->chunk_size_) == 0);
				// serialize the sample into the stream
				if (data_protocol_version_ >= 110) samp->save_streambuf(feedbuf_,data_protocol_version_,use_byte_order_,scratch_.get()); else *outarch_ << *samp;
				// if the sample shall be pushed though...
				if (samp->pushthrough) {
					// send off the chunk that we aggregated so far
					boost::unique_lock<boost::mutex> lock(completion_mut_);
					transfer_completed_ = false;
					async_write(*sock_,feedbuf_.data(),
						boost::bind(&client_session::handle_chunk_transfer_outcome,shared_from_this(),placeholders::error,placeholders::bytes_transferred));
					// wait for the completion condition
					completion_cond_.wait(lock, boost::bind(&client_session::transfer_completed,this));
					// handle transfer outcome
					if (!transfer_error_) {
						feedbuf_.consume(transfer_amount_);
					} else
						break;
				}

			} catch(std::exception &e) {
				std::cerr << "Unexpected glitch in transfer_samples_thread (id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
			}
		}
	} catch(std::exception &e) {
		std::cerr << "Unexpected error in transfer_samples_thread (id: " << boost::this_thread::get_id() << "): " << e.what() << "; exiting..." << std::endl;
	}
}

/// Handler that gets called when a sample transfer has been completed.
void tcp_server::client_session::handle_chunk_transfer_outcome(error_code err, std::size_t len) {
	try {
		{
			boost::lock_guard<boost::mutex> lock(completion_mut_);
			// assign the transfer outcome
			transfer_error_ = err;
			transfer_amount_ = len;
			transfer_completed_ = true;
		}
		// notify the server thread
		completion_cond_.notify_all();
	} catch(std::exception &e) {
		std::cerr << "Catastrophic error in handling the chunk transfer outcome (in tcp_server): " << e.what() << std::endl;
	}
}

