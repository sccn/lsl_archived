#ifndef RESOLVER_IMPL_H
#define RESOLVER_IMPL_H

#include "common.h"
#include "api_config.h"
#include "stream_info_impl.h"
#include "resolve_attempt_udp.h"
#include <set>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>


using boost::asio::ip::udp;
using boost::asio::ip::tcp;
using boost::system::error_code;

namespace lsl {

	/// Pointer to a deadline timer
	typedef boost::shared_ptr<boost::asio::deadline_timer> deadline_timer_p;
	/// Pointer to an io_service
	typedef boost::shared_ptr<boost::asio::io_service> io_service_p;

	/**
	* A stream resolver object.
	* Maintains the necessary resources for a resolve process, 
	* used by the free-standing resolve functions, the continuous_resolver class, and the inlets.
	*
	* A resolver instance can be operated in two different ways:
	* 1) In one shot: The resolver is queried one or more times by calling resolve_oneshot().
	* 2) Continuously: First a background query process is started that keeps updating a results list 
	*				   by calling resolve_continuous() and the list is retrieved in parallel when desired using
	*				   results(). In this case a new resolver instance must be created to issue a new query.
	*/
	class resolver_impl: public cancellable_registry, public boost::noncopyable {
	public:
		/**
		* Instantiate a new resolver and configure timing parameters.
		* A note on resolution logic. If KnownPeers in the api_config is empty, a new multicast wave will be scheduled every mcast_min_rtt 
		* (until a timeout expires or the desired number of streams has been resolved).If KnownPeers is non-empty, a multicast wave and a 
		* unicast wave will be schedule in alternation. The spacing between waves will be no shorter than the respective minimum RTTs.
		* In continuous mode a special set of timings that is somewhat more lax is used (see API config).
		*/
		resolver_impl();

		/// Destructor.
		/// Cancels any ongoing processes and waits until they finish.
		~resolver_impl();

		/**
		* Resolve a query string into a list of matching stream_info's on the network.
		* Blocks until at least the minimum number of streams has been resolved, or the timeout fires, or the resolve has been cancelled.
		* @param query The query string to send (usually a set of conditions on the properties of the stream info that should be searched,
		*              for example "name='BioSemi' and type='EEG'" (without the outer ""). See stream_info_impl::matches_query() for the 
		*			   definition of a query.
		* @param minimum The minimum number of unique streams that should be resolved before this function may to return.
		* @param timeout The timeout after which this function is forced to return (even if it did not produce the desired number of results).
		* @param minimum_time Search for matching streams for at least this much time (e.g., if multiple streams may be present).
		*/
		std::vector<stream_info_impl> resolve_oneshot(const std::string &query, int minimum=0, double timeout=FOREVER, double minimum_time=0.0);

		/**
		* Starts a background thread that resolves a query string and periodically updates the list of present streams.
		* After this, the resolver can *not* be repurposed for other queries or for oneshot operation (a new instance needs to be created for that).
		* @param query The query string to send (usually a set of conditions on the properties of the stream info that should be searched,
		*              for example "name='BioSemi' and type='EEG'" (without the outer ""). See stream_info_impl::matches_query() for the 
		*			   definition of a query.
		* @param forget_after If a stream vanishes from the network (e.g., because it was shut down), it will be pruned from the 
		*                     list this many seconds after it was last seen.
		* @param prune_interval Prune dead streams from the list every this many seconds.
		*/
		void resolve_continuous(const std::string &query, double forget_after=5.0);

		/// Get the current set of results (e.g., during continuous operation).
		std::vector<stream_info_impl> results();

		/// Tear down any ongoing operations and render the resolver unusable.
		/// This can be used to cancel a blocking resolve_oneshot() from another thread (e.g., to initiate teardown of the object).
		void cancel();

	private:
		/// This function starts a new wave of resolves.
		void next_resolve_wave();

		/// Start a new resolver attempt on the multicast hosts.
		void udp_multicast_burst();

		/// Start a new resolver attempt on the known peers.
		void udp_unicast_burst(error_code err);

		/// This handler is called when the overall timeout (if any) expires.
		void resolve_timeout_expired(error_code err);

		/// This handler is called when the wave timeout (if any) expires.
		void wave_timeout_expired(error_code err);

		/// Cancel the currently ongoing resolve, if any.
		void cancel_ongoing_resolve();


		// constants (mostly config-deduced)
		const api_config *cfg_;							// pointer to our configuration object
		std::vector<udp> udp_protocols_;				// UDP protocols under consideration
		std::vector<tcp> tcp_protocols_;				// TCP protocols under consideration
		std::vector<udp::endpoint> mcast_endpoints_;	// the list of multicast endpoints under consideration
		std::vector<udp::endpoint> ucast_endpoints_;	// the list of per-host UDP endpoints under consideration

		// things related to cancellation
		bool cancelled_;								// if set, no more resolves can be started (destructively cancelled).
		bool expired_;									// if set, ongoing operations will finished quickly

		// reinitialized for each query
		std::string query_;								// our current query string
		int minimum_;									// the minimum number of results that we want
		double forget_after_;							// forget results that are older than this (continuous operation only)
		double wait_until_;								// wait until this point in time before returning results (optional to allow for returning potentially more than a minimum number of results)
		bool fast_mode_;								// whether this is a fast resolve: determines the rate at which the query is repeated
		result_container results_;						// results are stored here
		boost::mutex results_mut_;						// a mutex that protects the results map

		// io objects
		io_service_p io_;								// our IO service
		boost::shared_ptr<boost::thread> background_io_;// a thread that runs background IO if we are performing a resolve_continuous
		boost::asio::deadline_timer resolve_timeout_expired_;	// the overall timeout for a query
		boost::asio::deadline_timer wave_timer_;		// a timer that fires when a new wave should be scheduled
		boost::asio::deadline_timer unicast_timer_;		// a timer that fires when the unicast wave should be scheduled
	};

}

#endif

