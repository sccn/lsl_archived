#ifndef RESOLVE_ATTEMPT_UDP_H
#define RESOLVE_ATTEMPT_UDP_H

#include "common.h"
#include "stream_info_impl.h"
#include "cancellation.h"
#include <map>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>


using boost::asio::ip::udp;
using boost::asio::ip::tcp;
using boost::system::error_code;

namespace lsl {

	/// A container for resolve results (map from stream instance UID onto (stream_info,receive-time)).
	typedef std::map<std::string,std::pair<stream_info_impl,double> > result_container;

	/// Pointer to a UDP resolve attempt.
	typedef boost::shared_ptr<class resolve_attempt_udp> resolve_attempt_udp_p;
	/// Pointer to a deadline timer
	typedef boost::shared_ptr<boost::asio::deadline_timer> deadline_timer_p;
	/// Pointer to an io_service
	typedef boost::shared_ptr<boost::asio::io_service> io_service_p;


	/**
	* An asynchronous resolve attempt for a single query targeted at a set of endpoints, via UDP.
	* A resolve attempt is an asynchronous operation submitted to an IO object, which amounts 
	* to a sequence of query packet sends (one for each endpoint in the list) and a sequence of 
	* result packet receives. The operation will wait for return packets until either a particular 
	* timeout has been reached or until it is cancelled via the cancel() method.
	*/
	class resolve_attempt_udp: public cancellable_obj, public boost::enable_shared_from_this<resolve_attempt_udp> {
		typedef std::vector<udp::endpoint> endpoint_list;
	public:
		/**
		* Instantiate and set up a new resolve attempt.
		* @param io The io_service that will run the async operations.
		* @param protocol The protocol (either udp::v4() or udp::v6()) to use for communications; 
		*				  only the subset of target addresses matching this protocol will be considered.
		* @param targets A list of udp::endpoint that should be targeted by this query.
		* @param query The query string to send (usually a set of conditions on the properties of the stream info that should be searched,
		*              for example "name='BioSemi' and type='EEG'" (without the outer ""). See stream_info_impl::matches_query() for the 
		*			   definition of a query.
		* @param results Reference to a container into which results are stored; potentially shared with other parallel resolve operations.
		*			     Since this is not thread-safe all operations modifying this must run on the same single-threaded IO service.
		* @param results_mut Reference to a mutex that protects the container.
		* @param cancel_after The time duration after which the attempt is automatically cancelled, i.e. the receives are ended.
		* @param registry A registry where the attempt can register itself as active so it can be cancelled during shutdown.
		*/
		resolve_attempt_udp(boost::asio::io_service &io, const udp &protocol, const std::vector<udp::endpoint> &targets, const std::string &query, result_container &results, boost::mutex &results_mut, double cancel_after=5.0, cancellable_registry *registry=NULL);

		/// Destructor
		~resolve_attempt_udp();

		/// Start the attempt asynchronously.
		void begin();

		/// Cancel operations asynchronously, and destructively.
		/// Note that this mostly serves to expedite the destruction of the object,
		/// which would happen anyway after some time.
		/// As the attempt instance is owned by the handler chains 
		/// the cancellation eventually leads to the destruction of the object.
		void cancel();

	private:
		// === send and receive handlers ===

		/// This function asks to receive the next result packet.
		void receive_next_result();

		/// Thos function starts an async send operation for the given current endpoint.
		void send_next_query(endpoint_list::const_iterator i);

		/// Handler that gets called when a receive has completed.
		void handle_receive_outcome(error_code err, std::size_t len);

		/// Handler that gets called when a send has completed.
		void handle_send_outcome(endpoint_list::const_iterator i, error_code err);

		// === cancellation ===

		/// Handler that gets called when the give up timeout has expired.
		void handle_timeout(error_code err);

		/// Cancel the outstanding operations.
		void do_cancel();


		// data shared with the resolver_impl
		boost::asio::io_service &io_;	// reference to the IO service that executes our actions
		result_container &results_;		// shared result container
		boost::mutex &results_mut_;		// shared mutex that protects the results

		// constant over the lifetime of this attempt
		double cancel_after_;			// the timeout for giving up
		bool cancelled_;				// whether the operation has been cancelled
		bool is_v4_;					// whether we're on IPv4 or IPv6
		udp protocol_;					// the protocol to use
		std::vector<udp::endpoint> targets_;// list of endpoints that should receive the query
		std::string query_;				// the query string
		std::string query_msg_;			// the query message that we're sending
		std::string query_id_;			// the (more or less) unique id for this query

		// data maintained/modified across handler invocations
		udp::endpoint remote_endpoint_; // the endpoint from which we received the last result
		char resultbuf_[65536];			// holds a single result received from the net

		// IO objects
		udp::socket unicast_socket_;	// socket to send data over (for unicasts)
		udp::socket broadcast_socket_;	// socket to send data over (for broadcasts)
		udp::socket multicast_socket_;	// socket to send data over (for multicasts)
		udp::socket recv_socket_;		// socket to receive replies (always unicast)
		boost::asio::deadline_timer cancel_timer_;	// timer to schedule the cancel action
	};

}

#endif

