#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include "common.h"
#include "stream_info_impl.h"
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>


using boost::asio::ip::udp;
using boost::system::error_code;

namespace lsl {

	/// shared pointer to a string
	typedef boost::shared_ptr<std::string> string_p;
	/// shared pointer to a UDP server object
	typedef boost::shared_ptr<class udp_server> udp_server_p;
	/// shared pointer to a socket
	typedef boost::shared_ptr<udp::socket> udp_socket_p;

	/*
	* A lightweight UDP responder service.
	* Understands the following messages:
	*  * LSL:shortinfo. This is a request for the stream_info that comes with a query string (and a return address). A packet is returned only if the query matches.
	*  * LSL:timedata.  This is a request for time synchronization info that comes with a time stamp (t0). The t0 stamp and two more time stamps (t1 and t2) are returned (similar to the NTP packet exchange).
	*/
	class udp_server: public boost::enable_shared_from_this<udp_server> {
	public:
		/*
		* Create a UDP responder that listens "side by side" with a TCP server.
		* This server will listen on a free local port for timedata and shortinfo requests -- mainly for timing information (unless shortinfo is needed by clients).
		* @param info The stream_info of the stream to serve (shared). After success, the appropriate service port will be assigned.
		* @param protocol The protocol stack to use (tcp::v4() or tcp::v6()).
		*/
		udp_server(const stream_info_impl_p &info, boost::asio::io_service &io, udp protocol);

		/**
		* Create a new UDP server in multicast mode.
		* This server will listen on a multicast address and responds only to LSL:shortinfo requests. This is for multicast/broadcast (and optionally unicast) local service discovery.
		*/
		udp_server(const stream_info_impl_p &info, boost::asio::io_service &io, const std::string &address, int port, int ttl, const std::string &listen_address);


		/// Start serving UDP traffic.
		/// Call this only after the (shared) info object has been initialized by every involved party.
		void begin_serving();

		/// Initiate teardown of UDP traffic.
		void end_serving();

	private:
		/// Initiate next packet request.
		/// The result of the operation will eventually trigger the handle_receive_outcome() handler.
		void request_next_packet();

		/// Handler that gets called when a the next packet was received (or the op was cancelled).
		void handle_receive_outcome(error_code err, std::size_t len);

		/// Handler that gets called after a response packet has been sent off.
		void handle_send_outcome(string_p replymsg, error_code err);

		stream_info_impl_p info_;			// stream_info reference
		boost::asio::io_service &io_;		// IO service reference
		udp_socket_p socket_;				// our socket

		char buffer_[65536];				// a buffer of data (we're receiving on it)
		bool time_services_enabled_;		// whether the time services are enabled
		udp::endpoint remote_endpoint_;		// the endpoint that we're currently talking to
		std::string shortinfo_msg_;			// pre-computed server response
	};
}

#endif
