#include "api_config.h"
#include "common.h"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>


// === implementation of the api_config class ===

using namespace lsl;
using namespace boost::filesystem;
using namespace boost::algorithm;

/// Helper function: Substitute the "~" character by the full home directory (according to environment variables).
path static expand_tilde(const std::string &filename) {
	if (!filename.empty() && filename[0] == '~') {
		if (getenv("HOME"))
			return path(getenv("HOME")) / path(filename.substr(1));
		if (getenv("USERPROFILE"))
			return path(getenv("USERPROFILE")) / path(filename.substr(1));
		if (getenv("HOMEDRIVE") && getenv("HOMEPATH"))
			return path(std::string(getenv("HOMEDRIVE")) + getenv("HOMEPATH")) / path(filename.substr(1));
		std::cerr << "Cannot determine the user's home directory; config files in the home directory will not be discovered." << std::endl;
	}
	return path(filename);
}

/// Helper function: Parse a set specifier (a string of the form {a, b, c, ...}) into a vector of strings.
static std::vector<std::string> parse_set(const std::string &setstr) {
	std::vector<std::string> result;
	if ((setstr.size() > 2) && setstr[0] == '{' && setstr[setstr.size()-1] == '}') {
		// non-empty set: split by ","
        std::string sub = setstr.substr(1,setstr.size()-2);
        boost::algorithm::split(result,sub,boost::algorithm::is_any_of(","));
		// remove leading and trailing whitespace from each element
		for (std::vector<std::string>::iterator i=result.begin(); i!=result.end(); i++)
			trim(*i);
	}
	return result;
}


/**
* Constructor.
* Applies default settings and overrides them based on a config file (if present).
*/
api_config::api_config() {
	// for each config file location under consideration...
	const char *filenames[] = {"lsl_api.cfg", "~/lsl_api/lsl_api.cfg", "/etc/lsl_api/lsl_api.cfg"};
	for (unsigned k=0; k < sizeof(filenames)/sizeof(filenames[0]); k++) {
		try {
			path p = expand_tilde(filenames[k]);
			if (exists(p)) {
				// try to load it if the file exists
				load_from_file(system_complete(p).string());
				// successful: finished
				return;
			}
		} catch(std::exception &e) {
			std::cerr << "Error trying to load config file " << filenames[k] << ": " << e.what() << std::endl;
		}
	}
	// unsuccessful: load default settings
	load_from_file();
}


/**
* Load a configuration file (or use defaults if a filename is empty).
* Expects a proper platform-native file name. Throws if there's an error.
*/
void api_config::load_from_file(const std::string &filename) {
	try {
		boost::property_tree::ptree pt;
		if (!filename.empty())
			read_ini(filename, pt);

		// read out the [ports] parameters
		multicast_port_ = pt.get("ports.MulticastPort",16571);
		base_port_ = pt.get("ports.BasePort",16572);
		port_range_ = pt.get("ports.PortRange",32);
		allow_random_ports_ = pt.get("ports.AllowRandomPorts",true);
#ifdef __APPLE__
		ipv6_ = pt.get("ports.IPv6","disable"); // on Mac OS (10.7) there's a bug in the IPv6 implementation that breaks LSL when it tries to use both v4 and v6
#else
		ipv6_ = pt.get("ports.IPv6","allow");
#endif
		// fix some common mis-spellings
		if (ipv6_ == "disabled")
			ipv6_ = "disable";
		if (ipv6_ == "allowed")
			ipv6_ = "allow";
		if (ipv6_ == "forced")
			ipv6_ = "force";
		if (ipv6_ != "disable" && ipv6_ != "allow" && ipv6_ != "force")
			throw std::runtime_error("Unsupported setting for the IPv6 parameter.");

		// read the [multicast] parameters
		resolve_scope_ = pt.get("multicast.ResolveScope","site");
		listen_address_ = pt.get("multicast.ListenAddress","");
		std::vector<std::string> machine_group = parse_set(pt.get("multicast.MachineAddresses","{127.0.0.1, FF31:113D:6FDD:2C17:A643:FFE2:1BD1:3CD2}"));
		std::vector<std::string> link_group = parse_set(pt.get("multicast.LinkAddresses","{255.255.255.255, 224.0.0.183, FF02:113D:6FDD:2C17:A643:FFE2:1BD1:3CD2}"));
		std::vector<std::string> site_group = parse_set(pt.get("multicast.SiteAddresses","{239.255.172.215, FF05:113D:6FDD:2C17:A643:FFE2:1BD1:3CD2}"));
		std::vector<std::string> organization_group = parse_set(pt.get("multicast.OrganizationAddresses","{239.192.172.215, FF08:113D:6FDD:2C17:A643:FFE2:1BD1:3CD2}"));
		std::vector<std::string> global_group = parse_set(pt.get("multicast.GlobalAddresses","{}"));

		multicast_ttl_ = -1;
		// construct list of addresses & TTL according to the ResolveScope.
		if (resolve_scope_ == "machine") {
			multicast_addresses_ = machine_group;
			multicast_ttl_ = 0;
		}
		if (resolve_scope_ == "link") {
			multicast_addresses_ = machine_group;
			multicast_addresses_.insert(multicast_addresses_.end(),link_group.begin(),link_group.end());
			multicast_ttl_ = 1;
		}
		if (resolve_scope_ == "site") {
			multicast_addresses_ = machine_group;
			multicast_addresses_.insert(multicast_addresses_.end(),link_group.begin(),link_group.end());
			multicast_addresses_.insert(multicast_addresses_.end(),site_group.begin(),site_group.end());
			multicast_ttl_ = 24;
		}
		if (resolve_scope_ == "organization") {
			multicast_addresses_ = machine_group;
			multicast_addresses_.insert(multicast_addresses_.end(),link_group.begin(),link_group.end());
			multicast_addresses_.insert(multicast_addresses_.end(),site_group.begin(),site_group.end());
			multicast_addresses_.insert(multicast_addresses_.end(),organization_group.begin(),organization_group.end());
			multicast_ttl_ = 32;
		}
		if (resolve_scope_ == "global") {
			multicast_addresses_ = machine_group;
			multicast_addresses_.insert(multicast_addresses_.end(),link_group.begin(),link_group.end());
			multicast_addresses_.insert(multicast_addresses_.end(),site_group.begin(),site_group.end());
			multicast_addresses_.insert(multicast_addresses_.end(),organization_group.begin(),organization_group.end());
			multicast_addresses_.insert(multicast_addresses_.end(),global_group.begin(),global_group.end());
			multicast_ttl_ = 255;
		}
		if (multicast_ttl_ == -1)
			throw std::runtime_error("This ResolveScope setting is unsupported.");

		// apply overrides, if any
		int ttl_override = pt.get("multicast.TTLOverride",-1);
		std::vector<std::string> address_override = parse_set(pt.get("multicast.AddressesOverride","{}"));
		if (ttl_override >= 0)
			multicast_ttl_ = ttl_override;
		if (!address_override.empty())
			multicast_addresses_ = address_override;

		// read the [lab] settings
		known_peers_ = parse_set(pt.get("lab.KnownPeers","{}"));
		session_id_ = pt.get("lab.SessionID","default");

		// read the [tuning] settings
		use_protocol_version_ = std::min(LSL_PROTOCOL_VERSION,pt.get("tuning.UseProtocolVersion",LSL_PROTOCOL_VERSION));
		watchdog_check_interval_ = pt.get("tuning.WatchdogCheckInterval",15.0);
		watchdog_time_threshold_ = pt.get("tuning.WatchdogTimeThreshold",15.0);
		multicast_min_rtt_ = pt.get("tuning.MulticastMinRTT",0.5);
		multicast_max_rtt_ = pt.get("tuning.MulticastMaxRTT",3.0);
		unicast_min_rtt_ = pt.get("tuning.UnicastMinRTT",0.75);
		unicast_max_rtt_ = pt.get("tuning.UnicastMaxRTT",5.0);
		continuous_resolve_interval_ = pt.get("tuning.ContinuousResolveInterval",0.5);
		timer_resolution_ = pt.get("tuning.TimerResolution",1);
		max_cached_queries_ = pt.get("tuning.MaxCachedQueries",100);
		time_update_interval_ = pt.get("tuning.TimeUpdateInterval",2.0);
		time_update_minprobes_ = pt.get("tuning.TimeUpdateMinProbes",6);
		time_probe_count_ = pt.get("tuning.TimeProbeCount",8);
		time_probe_interval_ = pt.get("tuning.TimeProbeInterval",0.064);
		time_probe_max_rtt_ = pt.get("tuning.TimeProbeMaxRTT",0.128);
		outlet_buffer_reserve_ms_ = pt.get("tuning.OutletBufferReserveMs",5000);
		outlet_buffer_reserve_samples_ = pt.get("tuning.OutletBufferReserveSamples",128);
		inlet_buffer_reserve_ms_ = pt.get("tuning.InletBufferReserveMs",5000);
		inlet_buffer_reserve_samples_ = pt.get("tuning.InletBufferReserveSamples",128);
		smoothing_halftime_ = pt.get("tuning.SmoothingHalftime",90.0f);
		force_default_timestamps_ = pt.get("tuning.ForceDefaultTimestamps", false);

	} catch(std::exception &e) {
		std::cerr << "Error parsing config file " << filename << " (" << e.what() << "). Rolling back to defaults." << std::endl;
		// any error: assign defaults
		load_from_file();
		// and rethrow
		throw e;
	}
}



/**
* Instantiate / retrieve singleton.
*/
const api_config *api_config::get_instance() {
	boost::call_once(&called_once,once_flag);
	return get_instance_internal();
}

api_config *api_config::get_instance_internal() {
	static api_config cfg;
	return &cfg;
}

void api_config::called_once() { get_instance_internal(); }

boost::once_flag api_config::once_flag = BOOST_ONCE_INIT;

