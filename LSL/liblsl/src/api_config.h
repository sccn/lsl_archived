#ifndef API_CONFIG_H
#define API_CONFIG_H

#include <string>
#include <vector>
#include <boost/thread/once.hpp>


namespace lsl {

	/**
	* A configuration object: holds all the configurable settings of liblsl.
	* These settings can be set via a configuration file that is automatically searched 
	* by stream providers and recipients in a series of locations:
	*  - lsl_api.cfg
	*  - ~/lsl_api/lsl_api.cfg
	*  - /etc/lsl_api/lsl_api.cfg
	*
	* Note that, while in some cases it might seem sufficient to override configurations
	* only for a subset of machines involved in a recording session (e.g., the servers), 
	* it is recommended that the same settings are used by all machines (stream recipients 
	* and providers) to avoid subtle bugs.
	*/
	class api_config {
	public:
		/**
		* Get a pointer to this singleton.
		*/
		static const api_config *get_instance();

		// === core parameters ===

		/*
		* Lowest port used to provide data streams & service information.
		* Up to port_range successively higher port numbers may be utilized,
		* depending on how many streams are being served on one machine.
		* If an outlet shall be reachable from outside a firewall, all TCP/UDP 
		* ports starting from base_port up to base_port+port_range-1, as well as 
		* the multicast_port should be open. If an inlet is behind a firewall,
		* the UDP ports starting from base_port up to base_port+port_range-1 should
		* be opened in order to allow for return packets in response to stream 
		* discovery queries.
		*/
		int base_port() const { return base_port_; }

		/**
		* Number of ports available on a machine for serving streams.
		* This is the number of ports, starting from the base_port that can be allocated for serving streams.
		* This limits the number of outlets that can coexist on a single machine to port_range; by increasing 
		* this number this limit can be expanded. 
		*/
		int port_range() const { return port_range_; }

		/**
		* Whether to allow binding to a randomly assigned port.
		* This can be used when the regular port range has been exhausted.
		*/
		int allow_random_ports() const { return allow_random_ports_; }

		/**
		* Port over which multi-cast communication is handled.
		* This is the communication medium for the announcement and discovery of streams 
		* between inlets and outlets. Note that according to the router configuration some 
		* multicast address ranges or ports may be blocked.
		*/
		int multicast_port() const { return multicast_port_; }

		/**
		* How the IPv6 protocol should be handled.
		* Can be "disable" (use only IPv4), "force" (use only IPv6), or "allow" (use both protocol stacks).
		*/ 
		const std::string &ipv6() const { return ipv6_; }

		/*
		* The range or scope of stream lookup when using multicast-based discovery; determines the output 
		* of the member functions multicast_addresses() and multicast_ttl(). Can take the values "machine", 
		* "link", "site", "organization", or "global".
		*/
		const std::string &resolve_scope() const { return resolve_scope_; }

		/*
		* List of multicast addresses on which inlets and outlets advertise/discover streams.
		*
		* This is merged from several other config file entries (LocalAddresses,SiteAddresses,OrganizationAddresses,
		* GlobalAddresses) goverened according to the ResolveScope setting.

		* Each participant in the network is aware of all addresses in this list, and will try all of them
		* if necessary. For smooth operation this list should ideally include both IPv4 and IPv6 addresses 
		* to work on networks on which one of the two is disabled. Specifically, the list should contain 
		* both the broadcast address 255.255.255.255 and link-local multicast addresses. To communicate across
		* routers within a site (depending on local policy, e.g., the department) or organization (e.g., the campus),
		* or at larger scope, multicast addresses with the according scope need to be included.
		*/
		const std::vector<std::string> &multicast_addresses() const { return multicast_addresses_; }

		/*
		* The address of the local interface on which to listen to multicast traffic.
		* The default is an empty string, i.e. bind to the default interface(s).
		*/
		const std::string &listen_address() const { return listen_address_; }

		/**
		* The TTL setting (time-to-live) for the multicast packets.
		* This is determined according to the ResolveScope setting if not overridden by the TTLOverride setting.
		* The higher this number (0-255), the broader their distribution. Routers (if correctly configured) 
		* employ various thresholds below which packets are not further forwarded. These are:
		*  0: Restricted to the same host -- not forwarded by a network card.
		*  1: Restricted to the same subnet -- not forwarded by a router.
		*  32: Restricted to the same site, organization or department.
		*  64: Restricted to the same region (definition of region varies).
		* 128: Restricted to the same continent.
		* 255: Not restricted in scope (global).
		*/
		int multicast_ttl() const { return multicast_ttl_; } 

		/**
		* The configured session ID. 
		* Allows to keep recording operations isolated from each other (precluding unwanted interference).
		*/
		const std::string &session_id() const { return session_id_; }

		/**
		* List of known host names that may provide LSL streams.
		* Can serve as a fallback if multicast/broadcast communication fails on a given network.
		*/
		const std::vector<std::string> &known_peers() const { return known_peers_; }

		// === tuning parameters ===

		/// The network protocol version to use.
		int use_protocol_version() const { return use_protocol_version_; }
		/// The interval at which the watchdog checks if connections are still fine.
		double watchdog_check_interval() const { return watchdog_check_interval_; }
		/// The watchdog takes no action if not at least this much time has passed since the last receipt of data. In seconds.
		double watchdog_time_threshold() const { return watchdog_time_threshold_; }
		/// The minimum assumed round-trip-time for a multicast query. Any subsequent packet wave would be started no earlier than this.
		double multicast_min_rtt() const { return multicast_min_rtt_; }
		/// The maximum assumed round-trip-time for a multicast query. We will stop waiting for return packets for a wave after this time.
		double multicast_max_rtt() const { return multicast_max_rtt_; }
		/// The minimum assumed round-trip-time for a multi-peer/multi-port unicast query. Any subsequent packet wave would be started no earlier than this.
		double unicast_min_rtt() const { return unicast_min_rtt_; }
		/// The maximum assumed round-trip-time for a multi-peer/multi-port unicast query.
		double unicast_max_rtt() const { return unicast_max_rtt_; }
		/// The interval at which resolve queries are emitted for continuous/background resolve activities. This is in addition to the assumed RTT's.
		double continuous_resolve_interval() const { return continuous_resolve_interval_; }
		/// Desired timer resolution in ms (0 means no change). Currently only affects Windows operating systems, where values other than 1 can increase LSL transmission latency.
		int timer_resolution() const { return timer_resolution_; }
		/// The maximum number of most-recently-used queries that is cached.
		int max_cached_queries() const { return max_cached_queries_; }
		/// Interval between background time correction updates.
		double time_update_interval() const { return time_update_interval_; }
		/// Minimum number of probes that must have been successful to perform a time update.
		int time_update_minprobes() const { return time_update_minprobes_; }
		/// Number of time probes that are being sent for a single update.
		int time_probe_count() const { return time_probe_count_; }
		/// Interval between the individual time probes that are sent to calculate an update.
		double time_probe_interval() const { return time_probe_interval_; }
		/// Maximum assumed RTT of a time probe (= extra waiting time).
		double time_probe_max_rtt() const { return time_probe_max_rtt_; }
		/// Default pre-allocated buffer size for the outlet, in ms (regular streams).
		int outlet_buffer_reserve_ms() const { return outlet_buffer_reserve_ms_; }
		/// Default pre-allocated buffer size for the outlet, in samples (irregular streams).
		int outlet_buffer_reserve_samples() const { return outlet_buffer_reserve_samples_; }
		/// Default pre-allocated buffer size for the inlet, in ms (regular streams).
		int inlet_buffer_reserve_ms() const { return inlet_buffer_reserve_ms_; }
		/// Default pre-allocated buffer size for the inlet, in samples (irregular streams).
		int inlet_buffer_reserve_samples() const { return inlet_buffer_reserve_samples_; }
		/// Default halftime of the time-stamp smoothing window (if enabled), in seconds.
		float smoothing_halftime() const { return smoothing_halftime_; }
		/// Override timestamps with lsl clock if True
		bool force_default_timestamps() const { return force_default_timestamps_; }

	private:
		// Thread-safe initialization logic (boilerplate).
		static boost::once_flag once_flag;
		static api_config *get_instance_internal();
		static void called_once();

		/// Constructor. Attempts to load a config file (according to precedence rules) or falls back to the default settings.
		api_config();

		/// Load a configuration file (or apply defaults if filename is empty).
		void load_from_file(const std::string &filename=std::string());

		/// Private copy constructor (noncopyable).
		api_config(const api_config &rhs);

		/// Private assignment operator (noncopyable).
		api_config &operator=(const api_config &rhs);

		// core parameters
		std::string ipv6_;
		int base_port_;
		int port_range_;
		bool allow_random_ports_;
		int multicast_port_;
		std::string resolve_scope_;
		std::vector<std::string> multicast_addresses_;
		int multicast_ttl_;
		std::string listen_address_;
		std::vector<std::string> known_peers_;
		std::string session_id_;
		// tuning parameters
		int use_protocol_version_;
		double watchdog_time_threshold_;
		double watchdog_check_interval_;
		double multicast_min_rtt_;
		double multicast_max_rtt_;
		double unicast_min_rtt_;
		double unicast_max_rtt_;
		double continuous_resolve_interval_;
		int timer_resolution_;
		int max_cached_queries_;
		double time_update_interval_;
		int time_update_minprobes_;
		int time_probe_count_;
		double time_probe_interval_;
		double time_probe_max_rtt_;
		int outlet_buffer_reserve_ms_;
		int outlet_buffer_reserve_samples_;
		int inlet_buffer_reserve_ms_;
		int inlet_buffer_reserve_samples_;
		float smoothing_halftime_;
		bool force_default_timestamps_;
	};
}

#endif

