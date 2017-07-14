#include <iostream>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "inlet_connection.h"
#include "api_config.h"


// === implementation of the inlet_connection class ===

using namespace lsl;
using namespace boost::asio;

/**
* Construct a new inlet connection.
* @param info A resolved stream info object (as coming from one of the resolver functions).
*			  It is possible -- but highly discouraged -- to initialize a connection with an unresolved (i.e. made-up) stream_info; in this case, 
*			  a connection will be resolved alongside based on the provided info, but will only succeed if the channel count and channel format 
*		      match the one that is provided.
* @param recover Try to silently recover lost streams that are recoverable (=those that that have a source_id set).
*				 In all other cases (recover is false or the stream is not recoverable) a lost_error is thrown where indicated if the stream's source is lost (e.g., due to an app or computer crash).
*/
inlet_connection::inlet_connection(const stream_info_impl &info, bool recover): type_info_(info), host_info_(info), recovery_enabled_(recover), tcp_protocol_(tcp::v4()), udp_protocol_(udp::v4()), lost_(false), shutdown_(false), last_receive_time_(lsl_clock()), active_transmissions_(0) {
	// if the given stream_info is already fully resolved...
	if (!host_info_.v4address().empty() || !host_info_.v6address().empty()) {

		// check LSL protocol version (we strictly forbid incompatible protocols instead of risking silent failure)
		if (type_info_.version()/100 > api_config::get_instance()->use_protocol_version()/100)
			throw std::runtime_error((std::string("The received stream (")+=host_info_.name()) += ") uses a newer protocol version than this inlet. Please update.");

		// select TCP/UDP protocol versions
		if (api_config::get_instance()->ipv6() == "allow") {
			// if IPv6 is optionally allowed...
			if (host_info_.v4address().empty() || !host_info_.v4data_port() || !host_info_.v4service_port()) {
				// then use it but only iff there are problems with the IPv4 connection data
				tcp_protocol_ = tcp::v6();
				udp_protocol_ = udp::v6();
			} else {
				// (otherwise stick to IPv4)
				tcp_protocol_ = tcp::v4();
				udp_protocol_ = udp::v4();
			}
		} else {
			// otherwise use the protocol type that is selected in the config
			tcp_protocol_ = (api_config::get_instance()->ipv6() == "force") ? tcp::v6() : tcp::v4();
			udp_protocol_ = (api_config::get_instance()->ipv6() == "force") ? udp::v6() : udp::v4();
		}

		if (recovery_enabled_ && type_info_.source_id().empty()) {
			// we cannot correctly recover streams which don't have a unique source id
			std::clog << "Note: The stream named '" << host_info_.name() << "' could not be recovered automatically if its provider crashed because it does not specify a unique data source ID." << std::endl;
			recovery_enabled_ = false;
		}

	} else {
		// the actual endpoint is not known yet -- we need to discover it later on the fly
		// check that all the necessary information for this has been fully specified
		if (type_info_.name().empty() && type_info_.type().empty() && type_info_.source_id().empty())
			throw std::invalid_argument("When creating an inlet with a constructed (instead of resolved) stream_info, you must assign at least the name, type or source_id of the desired stream.");
		if (type_info_.channel_count() == 0)
			throw std::invalid_argument("When creating an inlet with a constructed (instead of resolved) stream_info, you must assign a nonzero channel count.");
		if (type_info_.channel_format() == cf_undefined)
			throw std::invalid_argument("When creating an inlet with a constructed (instead of resolved) stream_info, you must assign a channel format.");

		// use the protocol that is specified in the config
		tcp_protocol_ = (api_config::get_instance()->ipv6() == "force") ? tcp::v6() : tcp::v4();
		udp_protocol_ = (api_config::get_instance()->ipv6() == "force") ? udp::v6() : udp::v4();

		// assign initial dummy endpoints
		host_info_.v4address("127.0.0.1");
		host_info_.v6address("::1");
		host_info_.v4data_port(49999);
		host_info_.v4service_port(49999);
		host_info_.v6data_port(49999);
		host_info_.v6service_port(49999);

		// recovery must generally be enabled
		recovery_enabled_ = true;
	}	
}

/// Engage the connection and its recovery watchdog thread.
void inlet_connection::engage() {
	if (recovery_enabled_)
		watchdog_thread_ = boost::thread(&inlet_connection::watchdog_thread,this);
}

/// Disengage the connection and all its resolver capabilities (including the watchdog).
void inlet_connection::disengage() {
	// shut down the connection
	{
		boost::lock_guard<boost::mutex> lock(shutdown_mut_);
		shutdown_ = true;
	}
	shutdown_cond_.notify_all();
	// cancel all operations (resolver, streams, ...)
	resolver_.cancel();
	cancel_and_shutdown();
	// and wait for the watchdog to finish
	if (recovery_enabled_)
		watchdog_thread_.join();
}


// === external accessors for connection properties ===

// get the TCP endpoint from the info (according to our configured protocol)
tcp::endpoint inlet_connection::get_tcp_endpoint() {
	boost::shared_lock<boost::shared_mutex> lock(host_info_mut_);
	
	if(tcp_protocol_ == tcp::v4()) {
        std::string address = host_info_.v4address();
        unsigned short port = host_info_.v4data_port();
        return tcp::endpoint(ip::address::from_string(address), port);
        
    //This more complicated procedure is required when the address is an ipv6 link-local address.
    //Simplified from https://stackoverflow.com/questions/10286042/using-boost-to-accept-on-ipv6-link-scope-address
	//It does not hurt when the address is not link-local.
	} else {
        std::string address = host_info_.v6address();
        std::string port = boost::lexical_cast<std::string>(host_info_.v6data_port());

        io_service io; 
        ip::tcp::resolver resolver(io);
        ip::tcp::resolver::query query( address, boost::lexical_cast<std::string>(port));
        ip::tcp::resolver::iterator it = resolver.resolve(query);
        ip::tcp::resolver::iterator end;
        
        if(it == end) {
            throw lost_error("Unable to resolve tcp stream at address: " + address + ", port: " + port);
        }
        //assuming first (typically only) element in list is valid.
        return *it;
    }
}

// get the UDP endpoint from the info (according to our configured protocol)
udp::endpoint inlet_connection::get_udp_endpoint() {
	boost::shared_lock<boost::shared_mutex> lock(host_info_mut_);
	
	if(udp_protocol_ == udp::v4()) {
        std::string address = host_info_.v4address();
        unsigned short port = host_info_.v4data_port();
        return udp::endpoint(ip::address::from_string(address), port);
        
    //This more complicated procedure is required when the address is an ipv6 link-local address.
    //Simplified from https://stackoverflow.com/questions/10286042/using-boost-to-accept-on-ipv6-link-scope-address
	//It does not hurt when the address is not link-local.
	} else {
        std::string address = host_info_.v6address();
        std::string port = boost::lexical_cast<std::string>(host_info_.v6data_port());

        io_service io; 
        ip::udp::resolver resolver(io);
        ip::udp::resolver::query query( address, boost::lexical_cast<std::string>(port));
        ip::udp::resolver::iterator it = resolver.resolve(query);
        ip::udp::resolver::iterator end;
        
        if(it == end) {
             throw lost_error("Unable to resolve udp stream at address: " + address + ", port: " + port);
        }
        //assuming first (typically only) element in list is valid.
        return *it;
    }
}

// get the hostname from the info
std::string inlet_connection::get_hostname() {
	boost::shared_lock<boost::shared_mutex> lock(host_info_mut_);
	return host_info_.hostname();
}

/// get the current stream UID (may change between crashes/reconnects)
std::string inlet_connection::current_uid() {
	boost::shared_lock<boost::shared_mutex> lock(host_info_mut_);
	return host_info_.uid();
}

/// get the current nominal sampling rate (might change between crashes/reconnects)
double inlet_connection::current_srate() {
	boost::shared_lock<boost::shared_mutex> lock(host_info_mut_);
	return host_info_.nominal_srate();
}


// === connection recovery logic ===

/// Performs the actual work of attempting a recovery.
void inlet_connection::try_recover() {
	if (recovery_enabled_) {
		try {
			boost::lock_guard<boost::mutex> lock(recovery_mut_);
			// first create the query string based on the known stream information
			std::ostringstream query; 
			{
				boost::shared_lock<boost::shared_mutex> lock(host_info_mut_);
				// construct query according to the fields that are present in the stream_info
				const char *channel_format_strings[] = {"undefined","float32","double64","string","int32","int16","int8","int64"};
				query << "channel_count='" << boost::lexical_cast<std::string>(host_info_.channel_count()) << "'";
				if (!host_info_.name().empty())
					query << " and name='" << host_info_.name() << "'";
				if (!host_info_.type().empty())
					query << " and type='" << host_info_.type() << "'";
				if (host_info_.nominal_srate() > 0)
					query << " and nominal_srate='" << boost::lexical_cast<std::string>(host_info_.nominal_srate()) << "'";
				if (!host_info_.source_id().empty())
					query << " and source_id='" << host_info_.source_id() << "'";					
				query << " and channel_format='" << channel_format_strings[host_info_.channel_format()] << "'";
			}
			// attempt a recovery
			for (int attempt=0;;attempt++) {
				// issue the resolve (blocks until it is either cancelled or got at least one matching streaminfo and has waited for a certain timeout)
				std::vector<stream_info_impl> infos = resolver_.resolve_oneshot(query.str(),1,FOREVER,attempt==0 ? 1.0 : 5.0);
				if (!infos.empty()) {
					// got a result
					boost::unique_lock<boost::shared_mutex> lock(host_info_mut_);
					// check if any of the returned streams is the one that we're currently connected to
					for (unsigned k=0;k<infos.size();k++)
						if (infos[k].uid() == host_info_.uid())
							return; // in this case there is no need to recover (we're still fine)
					// otherwise our stream is gone and we indeed need to recover:
					// ensure that the query result is unique (since someone might have used a non-unique stream ID)
					if (infos.size() == 1) {
						// update the endpoint
						host_info_ = infos[0];
						// cancel all cancellable operations registered with this connection
						cancel_all_registered();
						// invoke any callbacks associated with a connection recovery
						boost::lock_guard<boost::mutex> lock(onrecover_mut_);
						for(std::map<void*,boost::function<void()> >::iterator i=onrecover_.begin(),e=onrecover_.end();i!=e;i++)
							(i->second)();
					} else {
						// there are multiple possible streams to connect to in a recovery attempt: we warn and re-try
						// this is because we don't want to randomly connect to the wrong source without the user knowing about it;
						// the correct action (if this stream shall indeed have multiple instances) is to change the user code and 
						// make its source_id unique, or remove the source_id altogether if that's not possible (therefore disabling the ability to recover)
						std::clog << "Found multiple streams with name='" << host_info_.name() << "' and source_id='" << host_info_.source_id() << "'. Cannot recover unless all but one are closed." << std::endl;
						continue;
					}
				} else {
					// cancelled
				}
				break;
			}
		} catch(std::exception &e) {
			std::cerr << "A recovery attempt encountered an unexpected error: " << e.what() << std::endl;
		}
	}
}

/// A thread that periodically re-resolves the stream and checks if it has changed its location
void inlet_connection::watchdog_thread() {
	while(!lost_ && !shutdown_) {
		try {
			// we only try to recover if a) there are active transmissions and b) we haven't seen new data for some time
			{
				boost::unique_lock<boost::mutex> lock(client_status_mut_);
				if ((active_transmissions_ > 0) && (lsl_clock() - last_receive_time_ > api_config::get_instance()->watchdog_time_threshold())) {
					lock.unlock();
					try_recover();
				}
			}
			// instead of sleeping we're waiting on a condition variable for the sleep duration 
			// so that the watchdog can be cancelled conveniently
			{
				boost::unique_lock<boost::mutex> lock(shutdown_mut_);
				shutdown_cond_.wait_for(lock,boost::chrono::duration<double>(api_config::get_instance()->watchdog_check_interval()), boost::bind(&inlet_connection::shutdown,this));
			}
		} catch(std::exception &e) {
			std::cerr << "Unexpected hiccup in the watchdog thread: " << e.what() << std::endl;
		}
	}
}

/// Issue a recovery attempt if a connection loss was detected.
void inlet_connection::try_recover_from_error() {
	if (!shutdown_) {
		if (!recovery_enabled_) {
			// if the stream is irrecoverable it is now lost, 
			// so we need to notify the other inlet components
			lost_ = true;
			try {
				boost::lock_guard<boost::mutex> lock(client_status_mut_);
				for(std::map<void*,boost::condition_variable*>::iterator i=onlost_.begin(),e=onlost_.end();i!=e;i++)
					i->second->notify_all();
			} catch(std::exception &e) {
				std::cerr << "Unexpected problem while trying to issue a connection loss notification: " << e.what() << std::endl;
			}
			throw lost_error("The stream read by this inlet has been lost. To recover, you need to re-resolve the source and re-create the inlet.");
		} else
			try_recover();
	}
}


// === client status updates ===

/// Indicate that a transmission is now active.
void inlet_connection::acquire_watchdog() {
	boost::lock_guard<boost::mutex> lock(client_status_mut_);
	active_transmissions_++;
}

/// Indicate that a transmission has just completed.
void inlet_connection::release_watchdog() {
	boost::lock_guard<boost::mutex> lock(client_status_mut_);
	active_transmissions_--;
}

/// Update the time when the last content was received from the source
void inlet_connection::update_receive_time(double t) {
	boost::lock_guard<boost::mutex> lock(client_status_mut_);
	last_receive_time_ = t;
}

/// Register a condition variable that should be notified when a connection is lost
void inlet_connection::register_onlost(void *id, boost::condition_variable *cond) {
	boost::lock_guard<boost::mutex> lock(client_status_mut_);
	onlost_[id] = cond;
}

/// Unregister a condition variable
void inlet_connection::unregister_onlost(void *id) {
	boost::lock_guard<boost::mutex> lock(client_status_mut_);
	onlost_.erase(id);
}

/// Register a callback function that shall be called when a recovery has been performed
void inlet_connection::register_onrecover(void *id, const boost::function<void()> &func) {
	boost::lock_guard<boost::mutex> lock(onrecover_mut_);
	onrecover_[id] = func;
}

/// Unregister a recovery callback function
void inlet_connection::unregister_onrecover(void *id) {
	boost::lock_guard<boost::mutex> lock(onrecover_mut_);
	onrecover_.erase(id);
}
