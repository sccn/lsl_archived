#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#pragma warning (disable:4800)	// (inefficiently converting int to bool in portable_oarchive instantiation...)

#include "stream_info_impl.h"
#include <set>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "common.h"

#include "send_buffer.h"
#include "api_config.h"
#include "portable_archive/portable_oarchive.hpp"


using boost::asio::ip::tcp;
using boost::system::error_code;

namespace lsl {

	/// shared pointer to a string
	typedef boost::shared_ptr<std::string> string_p;
	/// shared pointer to a socket
	typedef boost::shared_ptr<tcp::socket> tcp_socket_p;
	/// shared pointer to an acceptor socket
	typedef boost::shared_ptr<tcp::acceptor> tcp_acceptor_p;
	/// shared pointer to a TCP socket
	typedef boost::shared_ptr<class tcp_server> tcp_server_p;
	/// pointer to an io_service
	typedef boost::shared_ptr<boost::asio::io_service> io_service_p;

	/**
	* The TCP data server.
	* Acts as a TCP server on a free port (in the configured port range), and understands the following messages:
	*  * LSL:streamfeed: A request to receive streaming data on the connection. The server responds with
	*				     the shortinfo, two samples filled with a test pattern, followed by samples until
	*					 the server outlet goes out of existence.
	*  * LSL:fullinfo: A request for the stream_info served by this server.
	*  * LSL:shortinfo: A request for the stream_info served by this server if matching the provided query string.
	*                   The short version of the stream_info (empty <desc> element) is returned.
	*/
	class tcp_server: public boost::enable_shared_from_this<tcp_server> {
	public:
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
		* @param factory A sample factory that is shared with other server objects.
		* @param protocol The protocol (IPv4 or IPv6) that shall be serviced by this server.
		* @param chunk_size The preferred chunk size, in samples. If 0, the pushthrough flag determines the effective chunking.
		*/
		tcp_server(const stream_info_impl_p &info, const io_service_p &io, const send_buffer_p &sendbuf, const sample::factory_p &factory, tcp protocol, int chunk_size);

		/// Begin serving TCP connections.
		/// Should not be called before info_ has been fully initialized by all involved parties (tcp_server, udp_server)
		/// since no modifications to the stream_info thereafter are permitted.
		void begin_serving();

		/// Initiate teardown of IO processes.
		/// The actual teardown will be performed by the IO thread that runs the operations of this server.
		void end_serving();

	private:
		/// shared pointer to a client session
		class client_session;
		typedef boost::shared_ptr<client_session> client_session_p;

		/// Start accepting a new connection.
		void accept_next_connection();

		/// Handler that is called when the accept has finished.
		void handle_accept_outcome(client_session_p newsession, error_code err);

		/// Register an in-flight (active) session socket with the server (so that we can close it when a shutdown is requested externally).
		void register_inflight_socket(const tcp_socket_p &sock);

		/// Unregister an in-flight session socket.
		void unregister_inflight_socket(const tcp_socket_p &sock);

		/// Post a close of all in-flight sockets.
		void close_inflight_sockets();

		/**
		* Active session with a TCP client.
		* A note on memory ownership:
		* - Generally, the stream_outlet maintains shared ownership of the tcp_server's, io_service's, and stream_info.
		* - At any point in time there are likely multiple request/handler chains in flight somewhere between the operating system,
		*   boost::asio, and the various handlers below. The handlers are set up such that any memory that may be referred to by them
		*	in the future is owned (shared) by the handler/callback function objects (this is what is encapsulated by the client_session
		*   instance). Their lifetime is managed by boost::asio and ends when the handler chain ends (e.g., is aborted). Since the TCP server
		*   is referred to (occasionally) by handler code, the tcp_server is owned also by the client_sessions, and therefore kept alive for as
		*   long as there is at least one request chain running.
		* - There is a per-session transfer thread (transfer_samples_thread) that owns the respective client_session and therefore the 
		*   TCP server, as well (since it may refer to it); it goes out of scope once the server is being shut down.
		* - The TCP server and client session also have shared ownership of the io_service (since in some cases some transfer threads
		*	can outlive the stream outlet, and so the io_service is still kept around until all sockets have been properly released).
		* - So memory is generally owned by the code (functors and stack frames) that needs to refer to it for the duration of the execution.
		*/
		class client_session: public boost::enable_shared_from_this<client_session> {
			typedef boost::shared_ptr<boost::asio::io_service::work> work_p;
		public:
			/// Instantiate a new session & its socket.
			client_session(const tcp_server_p &serv);

			/// Destructor. Unregisters the session from the server.
			~client_session();

			/// Get the socket of this session.
			tcp_socket_p socket();

			/// Begin processing this session (i.e., data transmission over the socket).
			void begin_processing();

		private:

			/// Handler that gets called when the reading of the 1st line (command line) of the inbound message finished.
			void handle_read_command_outcome(error_code err);

			/// Handler that gets called after finishing reading of the query line.
			void handle_read_query_outcome(error_code err);

			/// Handler that gets called after finishing the sending of a reply (nothing to do here).
			void handle_send_outcome(error_code err);

			/// Helper function to send a status message to the connected party.
			void send_status_message(const std::string &msg);

			/// Handler that gets called after finishing the sending of a message, holding a reference to the message.
			void handle_status_outcome(string_p msg, error_code err);

			/// Handler that gets called after finishing the reading of feedparameters.
			void handle_read_feedparams(int request_protocol_version, std::string request_uid, error_code err);

			/// Handler that gets called sending the feedheader has completed.
			void handle_send_feedheader_outcome(error_code err, std::size_t n);

			/// Transfers samples from the server's send buffer into the async send queues of the IO threads.
			void transfer_samples_thread(client_session_p sess);

			/// Handler that gets called when a sample transfer has been completed.
			void handle_chunk_transfer_outcome(error_code err, std::size_t len);

			/// There is a condition variable that is waiting on this condition in the inner transfer loop
			bool transfer_completed() const { return transfer_completed_; }

			bool registered_;					// whether we have registered ourselves at the server as active (so we need to unregister ourselves at destruction)
			io_service_p io_;					// shared pointer to IO service; ensures that the IO is still around by the time the serv_ and sock_ need to be destroyed
			tcp_server_p serv_;					// the server that is associated with this connection
			tcp_socket_p sock_;					// connection socket
			work_p work_;						// a RAII class indicating to the owning io_service that there is work to do even if no outstanding handler is present

			// data used by the transfer thread (and some other handlers)
			boost::asio::streambuf feedbuf_;	// this buffer holds the data feed generated by us
			boost::asio::streambuf requestbuf_;	// this buffer holds the request as received from the client (incrementally filled)
			boost::scoped_ptr<eos::portable_oarchive> outarch_;	// output archive (wrapped around the feed buffer)
			std::istream requeststream_;		// this is a stream on top of the request buffer for convenient parsing			
			boost::scoped_array<char> scratch_;	// scratchpad memory (e.g., for endianness conversion)
			int data_protocol_version_;			// protocol version to use for transmission
			int use_byte_order_;				// byte order to use (0=portable, 1234=little endian, 4321=big endian, 2134=PDP endian, not supported)
			int chunk_granularity_;				// our chunk granularity
			int max_buffered_;					// maximum number of samples buffered

			// data exchanged between the transfer completion handler and the transfer thread
			bool transfer_completed_;			// whether the current transfer has finished (possibly with an error)
			error_code transfer_error_;			// the outcome of the last chunk transfer
			std::size_t transfer_amount_;		// the amount of bytes transferred
			boost::mutex completion_mut_;		// a mutex that protects the completion data
			boost::condition_variable completion_cond_;	// a condition variable that signals completion
		};

		// data used by the transfer threads
		int chunk_size_;						// the chunk size to use (or 0)
		bool shutdown_;							// shutdown flag: tells the transfer thread that it should terminate itself asap

		// data shared with the outlet
		stream_info_impl_p info_;				// shared stream_info object
		io_service_p io_;						// shared ptr to IO service; ensures that the IO is still around by the time the acceptor needs to be destroyed
		sample::factory_p factory_;				// reference to the sample factory (which owns the samples)
		send_buffer_p send_buffer_;				// the send buffer, shared with other TCP's and the outlet

		// acceptor socket
		tcp_acceptor_p acceptor_;				// our server socket

		// registry of in-flight client sockets (for cancellation)
		std::set<tcp_socket_p> inflight_;		// registry of currently in-flight sockets
		boost::recursive_mutex inflight_mut_;	// mutex protecting the registry from concurrent access

		// some cached data
		std::string shortinfo_msg_;				// pre-computed short-info server response
		std::string fullinfo_msg_;				// pre-computed full-info server response
	};
}

#endif

