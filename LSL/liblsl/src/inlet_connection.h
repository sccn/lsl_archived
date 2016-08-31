#ifndef INLET_CONNECTION_H
#define INLET_CONNECTION_H

#include <map>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include "common.h"
#include "resolver_impl.h"
#include "cancellation.h"


using boost::asio::ip::tcp;
using boost::asio::ip::udp;

namespace lsl {

	/**
	* An inlet connection encapsulates the inlet's connection endpoint (for the other components of the inlet) 
	* and provides a recovery mechanism in case the connection breaks down (e.g., due to a remote computer crash).
	*
	* When a client of the connection (one of the other inlet components) experiences a connection loss it invokes the 
	* function try_recover_from_error() which attempts to update the endpoint to a valid state (possible once the stream is back online). 
	* Since in some cases a client might not be able to detect a connection loss and so would stall forever, the inlet_connection 
	* maintains a watchdog thread that periodically checks and recovers the connection state. Internally the recovery works by 
	* using the resolver to find the desired stream on the network again and updating the endpoint information if it has changed.
	*/
	class inlet_connection: public cancellable_registry {
	public:
		/**
		* Construct a new inlet connection.
		* @param info A stream info object (either coming from one of the resolver functions or constructed manually).
		* @param recover Try to silently recover lost streams that are recoverable (= those that that have a unique source_id set).
		*				 In all other cases (recover is false or the stream is not recoverable) the stream is declared lost in case of 
		*                a connection breakdown.
		*/
		inlet_connection(const stream_info_impl &info, bool recover=true);

		/// Prepare the connection and its auto-recovery thread.
		/// This is called once by the inlet after all other initialization.
		void engage();

		/// Shut down the connection and all its running operations.
		/// This is called once by the inlet before any component is destroyed to 
		/// ensure orderly shutdown.
		void disengage();


		// === endpoint information (might change if the connection is rehosted) ===

		/// Get the current TCP endpoint from the info (according to our configured protocol).
		tcp::endpoint get_tcp_endpoint();
		/// Get the current UDP endpoint from the info (according to our configured protocol).
		udp::endpoint get_udp_endpoint();
		/// Get the current hostname from the info.
		std::string get_hostname();

		/// Get the TCP protocol type.
		tcp tcp_protocol() const { return tcp_protocol_; }
		/// Get the UDP protocol type.
		udp udp_protocol() const { return udp_protocol_; }


		// === connection status info ===

		/// True if the connection has been irretrievably lost.
		bool lost() const { return lost_; }

		/// True if the connection is being shut down.
		bool shutdown() const { return shutdown_; }


		// === recovery control ===

		/// Try to recover the connection.
		/// This either blocks until it succeeds or declares the connection as lost (if recovery is disabled),
		/// and throws a lost error. Only call this when the connection is found to have broken down
		/// (e.g., socket error).
		void try_recover_from_error();


		// === client status info ===

		/// Indicate that a transmission is now active and requesting the watchdog.
		/// The recovery watchdog will be inactive while no transmission is requested.
		void acquire_watchdog();

		/// Indicate that a transmission has just completed.
		/// The recovery watchdog will be inactive while no transmission is requested.
		void release_watchdog();

		/// Inform the connection that content was received from the source (using lsl::lsl_clock()).
		/// If a sufficient amount of time has passed since the last call the watchdog thread will 
		/// try to recover the connection.
		void update_receive_time(double t);

		/// Register a condition variable that should be notified when a connection is lost
		void register_onlost(void *id, boost::condition_variable *cond);

		/// Unregister a condition variable from the set that is notified on connection loss
		void unregister_onlost(void *id);

		/// Register a callback function that shall be called when a recovery has been performed
		void register_onrecover(void *id, const boost::function<void()> &func);

		/// Unregister a recovery callback function
		void unregister_onrecover(void *id);


		// === misc properties of the connected stream ===

		/// Get info about the connected stream's type/format.
		/// This information is constant and will never change over the lifetime of the connection.
		const stream_info_impl &type_info() const { return type_info_; }

		/// Get the current stream instance UID (which would be different after a crash and restart of the data source).
		std::string current_uid();

		/// Get the nominal srate of the endpoint; we assume that this might possibly change between crashes/restarts 
		/// of the data source under some circumstances (although such behavior would be strongly discouraged).
		double current_srate();


	private:
        /// A thread that periodically checks whether the connection should be recovered.
        void watchdog_thread();

        /// A (potentially speculative) resolve-and-recover operation.
        void try_recover();

		// core connection properties
		const stream_info_impl type_info_;			// static/read-only information of the stream (type & format)
		stream_info_impl host_info_;				// the volatile information of the stream (addresses and ports); protected by a read/write mutex
		boost::shared_mutex host_info_mut_;			// a mutex to protect the state of the host_info (single-write/multiple-reader)
		tcp tcp_protocol_;							// the TCP protocol used (according to api_config)
		udp udp_protocol_;							// the UDP protocol used (according to api_config)
		bool recovery_enabled_;						// whether we would try to recover the stream if it is lost
		bool lost_;									// whether the stream is irrecoverably lost (set by try_recover_from_error if recovery is disabled)

		// internal watchdog thread (to detect dead connections)
		boost::thread watchdog_thread_;				// re-resolves the current connection speculatively

		// things related to the shutdown condition
		bool shutdown_;								// indicates to threads that we're shutting down
		boost::mutex shutdown_mut_;					// a mutex to protect the shutdown state
		boost::condition_variable shutdown_cond_;	// condition variable indicating that we're shutting down

		// things related to recovery
		resolver_impl resolver_;					// our resolver, in case we need it
		boost::mutex recovery_mut_;					// we allow only one recovery operation at a time
        
		// client status info for recovery & notification purposes
		std::map<void*,boost::condition_variable*> onlost_;		// a group of condition variables that should be notified when the connection is lost 
		std::map<void*,boost::function<void()> > onrecover_;	// a group of callback functions that should be invoked once the connection has been recovered
		double last_receive_time_;					// the last time when we received data from the server
		int active_transmissions_;					// the number of currently active transmissions (data or info)
		boost::mutex client_status_mut_;			// protects the client status info
		boost::mutex onrecover_mut_;				// protects the onrecover callback map
	};

}

#endif

