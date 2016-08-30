#include <iostream>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include "udp_server.h"
#include "api_config.h"
#include "socket_utils.h"


// === implementation of the udp_server class ===

using namespace lsl;
using namespace boost::asio;

/*
* Create a UDP responder in unicast mode that listens next to a TCP server.
* This server will listen on a free local port for timedata and shortinfo requests -- mainly for timing information (unless shortinfo is needed by clients).
* @param info The stream_info of the stream to serve (shared). After success, the appropriate service port will be assigned.
* @param protocol The protocol stack to use (tcp::v4() or tcp::v6()).
*/
udp_server::udp_server(const stream_info_impl_p &info, io_service &io, udp protocol): info_(info), io_(io), socket_(new udp::socket(io)), time_services_enabled_(true) {
	// open the socket for the specified protocol
	socket_->open(protocol);

	// bind to a free port
	int port = bind_port_in_range(*socket_,protocol);

	// assign the service port field
	if (protocol == udp::v4())
		info_->v4service_port(port);
	else
		info_->v6service_port(port);
}

/*
* Create a new UDP server in multicast mode.
* This server will listen on a multicast address and responds only to LSL:shortinfo requests. This is for multicast/broadcast local service discovery.
*/
udp_server::udp_server(const stream_info_impl_p &info, io_service &io, const std::string &address, int port, int ttl, const std::string &listen_address): info_(info), io_(io), socket_(new udp::socket(io)), time_services_enabled_(false) {
	ip::address addr = ip::address::from_string(address);
	bool is_broadcast = address=="255.255.255.255";

	// set up the endpoint where we listen (note: this is not yet the multicast address)
	udp::endpoint listen_endpoint;
	if (listen_address.empty()) {
		// pick the default endpoint
		if (addr.is_v4())
			listen_endpoint = udp::endpoint(udp::v4(), (unsigned short)port);
		else
			listen_endpoint = udp::endpoint(udp::v6(), (unsigned short)port);
	}
	else {
		// choose an endpoint explicitly
		ip::address listen_addr = ip::address::from_string(listen_address);
		listen_endpoint = udp::endpoint(listen_addr, (unsigned short)port);
	}

	// open the socket and make sure that we can reuse the address, and bind it
	socket_->open(listen_endpoint.protocol());
	socket_->set_option(udp::socket::reuse_address(true));

	// set the multicast TTL
	if (addr.is_multicast() && !is_broadcast)
		socket_->set_option(ip::multicast::hops(ttl));

	// bind to the listen endpoint
	socket_->bind(listen_endpoint);

	// join the multicast group, if any
	if (addr.is_multicast() && !is_broadcast) {
		if (addr.is_v4())
			socket_->set_option(ip::multicast::join_group(addr.to_v4(),listen_endpoint.address().to_v4()));
		else
			socket_->set_option(ip::multicast::join_group(addr));
    }
}


// === externally issued asynchronous commands ===

/// Start serving UDP traffic.
/// Call this only after the (shared) info object has been initialized by all other parties, too.
void udp_server::begin_serving() {
	// pre-calculate the shortinfo message (now that everyone should have initialized their part).
	shortinfo_msg_ = info_->to_shortinfo_message();
	// start asking for a packet
	request_next_packet();
}

/// Initiate teardown of UDP traffic.
void udp_server::end_serving() {
	// gracefully close the socket; this will eventually lead to the cancellation of the IO operation(s) tied to its socket
	io_.post(boost::bind(&close_if_open<udp_socket_p>,socket_));
}


// === receive / reply loop ===

/// Initiate next packet request.
/// The result of the operation will eventually trigger the handle_receive_outcome() handler.
void udp_server::request_next_packet() {
	socket_->async_receive_from(boost::asio::buffer(buffer_), remote_endpoint_,
		boost::bind(&udp_server::handle_receive_outcome, shared_from_this(), placeholders::error, placeholders::bytes_transferred));
}

/// Handler that gets called when the next packet was received (or the op was cancelled).
void udp_server::handle_receive_outcome(error_code err, std::size_t len) {
	if (err != error::operation_aborted && err != error::shut_down) {
		try {
			if (!err) {
				// remember the time of packet reception for possible later use
				double t1 = time_services_enabled_ ? lsl_clock() : 0.0;

				// wrap received packet into a request stream and parse the method from it
				std::istringstream request_stream(std::string(buffer_,buffer_+len));
				std::string method; getline(request_stream,method); boost::trim(method);
				if (method == "LSL:shortinfo") {
					// shortinfo request: parse content query string
					std::string query; getline(request_stream,query); boost::trim(query);
					// parse return address, port, and query ID
					int return_port; request_stream >> return_port;
					std::string query_id; request_stream >> query_id;
					// check query
					if (info_->matches_query(query)) {
						// query matches: send back reply
						udp::endpoint return_endpoint(remote_endpoint_.address(),(unsigned short)return_port);
						string_p replymsg(new std::string((query_id += "\r\n") += shortinfo_msg_));
						socket_->async_send_to(boost::asio::buffer(*replymsg), return_endpoint,
							boost::bind(&udp_server::handle_send_outcome,shared_from_this(),replymsg,placeholders::error));
						return;
					}
				} else {
					if (time_services_enabled_ && method == "LSL:timedata") {
						// timedata request: parse time of original transmission
						int wave_id; request_stream >> wave_id;
						double t0; request_stream >> t0;
						// send it off (including the time of packet submission and a shared ptr to the message content owned by the handler)
						std::ostringstream reply; reply.precision(16); reply << " " << wave_id << " " << t0 << " " << t1 << " " << lsl_clock();
						string_p replymsg(new std::string(reply.str()));
						socket_->async_send_to(boost::asio::buffer(*replymsg), remote_endpoint_,
							boost::bind(&udp_server::handle_send_outcome,shared_from_this(),replymsg,placeholders::error));
						return;
					}
				}
			}
		} catch(std::exception &e) {
			std::cerr << "udp_server: hiccup during request processing: " << e.what() << std::endl;
		}
		request_next_packet();
	}
}

/// Handler that's called after a response packet has been sent off.
void udp_server::handle_send_outcome(string_p /*replymsg*/, error_code err) {
	if (err != error::operation_aborted && err != error::shut_down)
		// done sending: ask for next packet
		request_next_packet();
}


