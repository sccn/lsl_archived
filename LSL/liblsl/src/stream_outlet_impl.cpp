#define NO_EXPLICIT_TEMPLATE_INSTANTIATION // a convention that applies when including portable_oarchive.h in multiple .cpp files.
#include "stream_outlet_impl.h"
#include <boost/bind.hpp>


// === implementation of the stream_outlet_impl class ===

using namespace lsl;
using namespace boost::asio;

/**
* Establish a new stream outlet. This makes the stream discoverable.
* @param info The stream information to use for creating this stream stays constant over the lifetime of the outlet.
* @param chunk_size The preferred chunk size, in samples, at which data shall be transmitted over the network. Can be selectively overridden by the inlet.
*					If 0 (=default), the chunk size is determined by the pushthrough flag in push_sample or push_chunk.
* @param max_capacity The maximum number of samples buffered for unresponsive receivers. If more samples get pushed, the oldest will be dropped.
*					   The default is sufficient to hold a bit more than 15 minutes of data at 512Hz, while consuming not more than ca. 512MB of RAM.
*/
stream_outlet_impl::stream_outlet_impl(const stream_info_impl &info, int chunk_size, int max_capacity): chunk_size_(chunk_size), info_(new stream_info_impl(info)), 
	sample_factory_(new sample::factory(info.channel_format(),info.channel_count(),info.nominal_srate()?info.nominal_srate()*api_config::get_instance()->outlet_buffer_reserve_ms()/1000:api_config::get_instance()->outlet_buffer_reserve_samples())), send_buffer_(new send_buffer(max_capacity))
{
	ensure_lsl_initialized();
	const api_config *cfg = api_config::get_instance();

	// instantiate IPv4 and/or IPv6 stacks (depending on settings)
	if (cfg->ipv6() == "disable")
		instantiate_stack(tcp::v4(),udp::v4());
	if (cfg->ipv6() == "force")
		instantiate_stack(tcp::v6(),udp::v6());
	if (cfg->ipv6() == "allow") {
		try {
			instantiate_stack(tcp::v4(),udp::v4());
		} catch(std::exception &e) {
			std::cerr << "Could not instantiate IPv4 stack: " << e.what() << std::endl;
		}
		try {
			instantiate_stack(tcp::v6(),udp::v6());
		} catch(std::exception &e) {
			std::cerr << "Could not instantiate IPv6 stack: " << e.what() << std::endl;
		}
	}
	// fail if both stacks failed to instantiate
	if (tcp_servers_.empty() || udp_servers_.empty())
		throw std::runtime_error("Neither the IPv4 nor the IPv6 stack could be instantiated.");

	// get the async request chains set up
	for (unsigned k=0;k<tcp_servers_.size();k++)
		tcp_servers_[k]->begin_serving();
	for (unsigned k=0;k<udp_servers_.size();k++)
		udp_servers_[k]->begin_serving();
	for (unsigned k=0;k<responders_.size();k++)
		responders_[k]->begin_serving();

	// and start the IO threads to handle them
	for (unsigned k=0;k<ios_.size();k++)
		io_threads_.push_back(thread_p(new boost::thread(boost::bind(&stream_outlet_impl::run_io,this,ios_[k]))));
}

/**
* Instantiate a new server stack.
*/
void stream_outlet_impl::instantiate_stack(tcp tcp_protocol, udp udp_protocol) {
	// get api_config
	const api_config *cfg = api_config::get_instance();
	std::string listen_address = cfg->listen_address();
	std::vector<std::string> multicast_addrs = cfg->multicast_addresses();
	int multicast_ttl = cfg->multicast_ttl();
	int multicast_port = cfg->multicast_port();
	// create TCP data server
	ios_.push_back(io_service_p(new io_service()));
	tcp_servers_.push_back(tcp_server_p(new tcp_server(info_, ios_.back(), send_buffer_, sample_factory_, tcp_protocol, chunk_size_)));
	// create UDP time server
	ios_.push_back(io_service_p(new io_service()));
	udp_servers_.push_back(udp_server_p(new udp_server(info_, *ios_.back(), udp_protocol)));
	// create UDP multicast responders
	for (std::vector<std::string>::iterator i=multicast_addrs.begin(); i != multicast_addrs.end(); i++) {
		try {
			// use only addresses for the protocol that we're supposed to use here
			ip::address address(ip::address::from_string(*i));
			if (udp_protocol == udp::v4() ? address.is_v4() : address.is_v6())
				responders_.push_back(udp_server_p(new udp_server(info_, *ios_.back(), *i, multicast_port, multicast_ttl, listen_address)));
		} catch(std::exception &e) {
			std::clog << "Note (minor): could not create multicast responder for address " << *i << " (failed with: " << e.what() << ")" << std::endl;
		}
	}
}

/**
* Destructor.
* The stream will no longer be discoverable after destruction and all paired inlets will stop delivering data.
*/
stream_outlet_impl::~stream_outlet_impl() {
	try {
		// cancel all request chains
		for (unsigned k=0;k<tcp_servers_.size();k++)
			tcp_servers_[k]->end_serving();
		for (unsigned k=0;k<udp_servers_.size();k++)
			udp_servers_[k]->end_serving();
		for (unsigned k=0;k<responders_.size();k++)
			responders_[k]->end_serving();
		// join the IO threads
		for (unsigned k=0;k<io_threads_.size();k++)
			if (!io_threads_[k]->try_join_for(boost::chrono::milliseconds(1000))) {
 				// .. using force, if necessary (should only ever happen if the CPU is maxed out)
				std::cerr << "Tearing down stream_outlet of thread " << io_threads_[k]->get_id() << " (in id: " << boost::this_thread::get_id() << "): " << std::endl;
				ios_[k]->stop();
				for (int attempt=1; !io_threads_[k]->try_join_for(boost::chrono::milliseconds(1000)); attempt++) {
					std::cerr << "Trying to kill stream_outlet (attempt #" << attempt << ")..." << std::endl;
					io_threads_[k]->interrupt();
				}
			}
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during destruction of a stream outlet (id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "Severe error during stream outlet shutdown." << std::endl;
	}
}

// Run an IO service.
void stream_outlet_impl::run_io(io_service_p &ios) {
	while (true) {
		try {
			ios->run();
			return;
		} catch(std::exception &e) {
			std::cerr << "Error during io_service processing (id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
		}
	}
}

/**
* Retrieve the stream info associated with this outlet.
* This is the constant meta-data header that was used to create the stream.
*/
const stream_info_impl &stream_outlet_impl::info() const { return *info_; }

/**
* Check whether consumers are currently registered.
*/
bool stream_outlet_impl::have_consumers() { return send_buffer_->have_consumers(); }

/**
* Wait until some consumer shows up.
* @return Whether the wait was successful.
*/
bool stream_outlet_impl::wait_for_consumers(double timeout) { return send_buffer_->wait_for_consumers(timeout); }

