#include <sstream>
#include <iostream>
#include <istream>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/functional/hash.hpp>
#include "resolver_impl.h"


// === implementation of the resolver_impl class ===

using namespace lsl;
using namespace boost::asio;
using boost::posix_time::millisec;

/**
* Instantiate a new resolver and configure timing parameters.
* A note on resolution logic. If KnownPeers in the api_config is empty, a new multicast wave will be scheduled every mcast_min_rtt (until a timeout expires or the desired number of streams has been resolved).
* If KnownPeers is non-empty, a multicast wave an a unicast wave will be schedule in alternation. The spacing between waves will be no shorter than the respective minimum RTTs.
* TCP resolves are currently not implemented (but may be at a later time); these are only necessary when UDP traffic is disabled on a particular router.
*/
resolver_impl::resolver_impl(): cfg_(api_config::get_instance()), cancelled_(false), expired_(false), forget_after_(FOREVER), fast_mode_(true),  
	io_(io_service_p(new io_service())), resolve_timeout_expired_(*io_), wave_timer_(*io_), unicast_timer_(*io_) 
{
	// parse the multicast addresses into endpoints and store them
	std::vector<std::string> mcast_addrs = cfg_->multicast_addresses();
	int mcast_port = cfg_->multicast_port();
	for (unsigned k=0;k<mcast_addrs.size();k++) {
		try {
			mcast_endpoints_.push_back(udp::endpoint(ip::address::from_string(mcast_addrs[k]),(unsigned short)mcast_port));
		} 
		catch(std::exception &) { }
	}

	// parse the per-host addresses into endpoints, and store them, too
	std::vector<std::string> peers = cfg_->known_peers();
	udp::resolver udp_resolver(*io_);
	// for each known peer...
    for (unsigned k=0;k<peers.size();k++) {
        try {
            // resolve the name
            udp::resolver::query q(peers[k],boost::lexical_cast<std::string>(cfg_->base_port()));
            // for each endpoint...
            for (udp::resolver::iterator i=udp_resolver.resolve(q); i != udp::resolver::iterator(); i++) {
                // for each port in the range...
                for (int p=cfg_->base_port(); p<cfg_->base_port()+cfg_->port_range(); p++)
                    // add a record
                    ucast_endpoints_.push_back(udp::endpoint(i->endpoint().address(),p));
            }
        } catch(std::exception &) { }
    }

	// generate the list of protocols to use
	if (cfg_->ipv6() == "force" || cfg_->ipv6() == "allow") {
		udp_protocols_.push_back(udp::v6());
		tcp_protocols_.push_back(tcp::v6());
	}
	if (cfg_->ipv6() == "disable" || cfg_->ipv6() == "allow") {
		udp_protocols_.push_back(udp::v4());
		tcp_protocols_.push_back(tcp::v4());
	}
}


// === resolve functions ===

/**
* Resolve a query string into a list of matching stream_info's on the network.
* Blocks until at least the minimum number of streams has been resolved, or the timeout fires, or the resolve has been cancelled.
*/
std::vector<stream_info_impl> resolver_impl::resolve_oneshot(const std::string &query, int minimum, double timeout, double minimum_time) {
	// reset the IO service & set up the query parameters
	io_->reset();
	query_ = query;
	minimum_ = minimum;
	wait_until_ = lsl_clock() + minimum_time;
	results_.clear();
	forget_after_ = FOREVER;
	fast_mode_ = true;
	expired_ = false;

	// start a timer that cancels all outstanding IO operations and wave schedules after the timeout has expired
	if (timeout != FOREVER) {
		resolve_timeout_expired_.expires_from_now(millisec(1000*timeout));
		resolve_timeout_expired_.async_wait(boost::bind(&resolver_impl::resolve_timeout_expired,this,placeholders::error));
	}

	// start the first wave of resolve packets
	next_resolve_wave();

	// run the IO operations until finished
	if (!cancelled_) {
		io_->run();
		// collect output
		std::vector<stream_info_impl> output;
		for(result_container::iterator i=results_.begin(); i!= results_.end();i++)
			output.push_back(i->second.first);
		return output;
	} else
		return std::vector<stream_info_impl>();
}

void resolver_impl::resolve_continuous(const std::string &query, double forget_after) {
	// reset the IO service & set up the query parameters
	io_->reset();
	query_ = query;
	minimum_ = 0;
	wait_until_ = 0;
	results_.clear();
	forget_after_ = forget_after;
	fast_mode_ = false;
	expired_ = false;
	// start a wave of resolve packets
	next_resolve_wave();
	// spawn a thread that runs the IO operations
	background_io_.reset(new boost::thread(boost::bind(&io_service::run,io_)));
}

/// Get the current set of results (e.g., during continuous operation).
std::vector<stream_info_impl> resolver_impl::results() {
	std::vector<stream_info_impl> output;
	boost::lock_guard<boost::mutex> lock(results_mut_);
	double expired_before = lsl_clock() - forget_after_;
	for(result_container::iterator i=results_.begin(); i!=results_.end();) {
		if (i->second.second < expired_before) {
			result_container::iterator tmp = i++;
			results_.erase(tmp);
		} else {
			output.push_back(i->second.first);
			i++;
		}
	}
	return output;
}

// === timer-driven async handlers ===

/// This function starts a new wave of resolves.
void resolver_impl::next_resolve_wave() {
	std::size_t num_results = 0;
	{
		boost::lock_guard<boost::mutex> lock(results_mut_);
		num_results = results_.size();
	}
	if (cancelled_ || expired_ || (minimum_ && (num_results >= (std::size_t)minimum_) && lsl_clock() >= wait_until_)) {
		// stopping criteria satisfied: cancel the ongoing operations
		cancel_ongoing_resolve();
	} else {
		// start a new multicast wave
		udp_multicast_burst();
		if (!ucast_endpoints_.empty()) {
			// we have known peer addresses: we spawn a unicast wave and shortly thereafter the next wave
			unicast_timer_.expires_from_now(millisec(1000*cfg_->multicast_min_rtt()));
			unicast_timer_.async_wait(boost::bind(&resolver_impl::udp_unicast_burst,this,placeholders::error));
			wave_timer_.expires_from_now(millisec(1000*((fast_mode_?0:cfg_->continuous_resolve_interval())+(cfg_->multicast_min_rtt()+cfg_->unicast_min_rtt()))));
			wave_timer_.async_wait(boost::bind(&resolver_impl::wave_timeout_expired,this,placeholders::error));
		} else {
			// there are no known peer addresses; just set up the next wave
			wave_timer_.expires_from_now(millisec(1000*((fast_mode_?0:cfg_->continuous_resolve_interval())+cfg_->multicast_min_rtt())));
			wave_timer_.async_wait(boost::bind(&resolver_impl::wave_timeout_expired,this,placeholders::error));
		}
	}
}

/// Start a new resolver attepmpt on the multicast hosts.
void resolver_impl::udp_multicast_burst() {
		// start one per IP stack under consideration
		for (unsigned k=0,failures=0;k<udp_protocols_.size();k++) {
			try {
				resolve_attempt_udp_p attempt(new resolve_attempt_udp(*io_,udp_protocols_[k],mcast_endpoints_,query_,results_,results_mut_,cfg_->multicast_max_rtt(),this));
				attempt->begin();
			} catch(std::exception &e) {
				if (++failures == udp_protocols_.size())
					std::cerr << "Could not start a multicast resolve attempt for any of the allowed protocol stacks: " << e.what() << std::endl;
			}
		}

}

/// Start a new resolver attempt on the known peers.
void resolver_impl::udp_unicast_burst(error_code err) {
	if (err != error::operation_aborted) {
		// start one per IP stack under consideration
		for (unsigned k=0,failures=0;k<udp_protocols_.size();k++) {
			try {
				resolve_attempt_udp_p attempt(new resolve_attempt_udp(*io_,udp_protocols_[k],ucast_endpoints_,query_,results_,results_mut_,cfg_->unicast_max_rtt(),this));
				attempt->begin();
			} catch(std::exception &e) {
				if (++failures == udp_protocols_.size())
					std::cerr << "Could not start a unicast resolve attempt for any of the allowed protocol stacks: " << e.what() << std::endl;
			}
		}
	}
}

/// This handler is called when the overall resolve timeout (if any) expires.
void resolver_impl::resolve_timeout_expired(error_code err) {
	if (err != error::operation_aborted)
		cancel_ongoing_resolve();
}

/// This handler is called when the wave timeout expires.
void resolver_impl::wave_timeout_expired(error_code err) {
	if (err != error::operation_aborted)
		next_resolve_wave();
}


// === cancellation and teardown ===

/// Cancel any ongoing operations and render the resolver unusable.
/// This can be used to cancel a blocking resolve_oneshot() from another thread (e.g., to initiate teardown of the object).
void resolver_impl::cancel() {
	cancelled_ = true;
	cancel_ongoing_resolve();
}

/// Cancel an ongoing resolve, if any (otherwise without effect).
void resolver_impl::cancel_ongoing_resolve() {
	// make sure that ongoing handler loops terminate
	expired_ = true;
	// timer fires: cancel the next wave schedule
	io_->post(boost::bind(&deadline_timer::cancel,&wave_timer_));
	io_->post(boost::bind(&deadline_timer::cancel,&unicast_timer_));
	// and cancel the timeout, too
	io_->post(boost::bind(&deadline_timer::cancel,&resolve_timeout_expired_));
	// cancel all currently active resolve attempts
	cancel_all_registered();
}

/// Destructor.
/// Cancels any ongoing processes and waits until they finish.
resolver_impl::~resolver_impl() {
	try {
		if (background_io_) {
			cancel();
			background_io_->join();
		}
	} 
	catch(std::exception &e) {
		std::cerr << "Error during destruction of a resolver_impl: " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "Severe error during destruction of a resolver_impl." << std::endl;
	}
}

