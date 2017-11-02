#include <iostream>
#include <boost/serialization/string.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include "data_receiver.h"
#include "socket_utils.h"
#include "portable_archive/portable_iarchive.hpp"


// === implementation of the data_receiver class ===

using namespace lsl;
using namespace boost::algorithm;

/**
* Construct a new data receiver from an info connection.
* @param conn An inlet connection object.
* @param max_buflen Optionally the maximum amount of data to buffer (in seconds if there is a nominal sampling rate, otherwise x 100 in samples).
*					Recording applications want to use a fairly large buffer size here, while real-time applications want to only buffer as much as they need to perform their next calculation.
* @param max_chunklen Optionally the maximum size, in samples, at which chunks are transmitted (the default corresponds to the chunk sizes used by the sender).
*					  Recording applications can use a generous size here (leaving it to the network how to pack things), while real-time applications may want a finer (perhaps 1-sample) granularity.
*/
data_receiver::data_receiver(inlet_connection &conn, int max_buflen, int max_chunklen): conn_(conn), check_thread_start_(true), closing_stream_(false), connected_(false), sample_queue_(max_buflen), 
	sample_factory_(new sample::factory(conn.type_info().channel_format(),conn.type_info().channel_count(),conn.type_info().nominal_srate()?conn.type_info().nominal_srate()*api_config::get_instance()->inlet_buffer_reserve_ms()/1000:api_config::get_instance()->inlet_buffer_reserve_samples())), max_buflen_(max_buflen), max_chunklen_(max_chunklen) 
{
	if (max_buflen < 0)
		throw std::invalid_argument("The max_buflen argument must not be smaller than 0.");
	if (max_chunklen < 0)
		throw std::invalid_argument("The max_chunklen argument must not be smaller than 0.");
	conn_.register_onlost(this,&connected_upd_);
}

/// Destructor. Stops the background activities.
data_receiver::~data_receiver() {
	try {
		conn_.unregister_onlost(this);
		if (data_thread_.joinable())
			data_thread_.join();
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during destruction of a data_receiver: " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "Severe error during data receiver shutdown." << std::endl;
	}
}


// === external access ===

/**
* Open a new data stream.
* All samples pushed in at the other end from this moment onwards will be queued and
* eventually be delivered in response to pull_sample() or pull_chunk() calls.
* A pull call without preceding open_stream serves as an implicit open_stream.
* @param timeout Optional timeout of the operation (default: no timeout).
* @throws timeout_error (if the timeout expires), or lost_error (if the stream source has been lost).
*/
void data_receiver::open_stream(double timeout) {
	closing_stream_ = false;
	boost::unique_lock<boost::mutex> lock(connected_mut_);
	if (!connection_completed()) {
		// start thread if not yet running
		if (check_thread_start_ && !data_thread_.joinable()) {
			data_thread_ = boost::thread(&data_receiver::data_thread,this);
			check_thread_start_ = false;
		}
		// wait until the connection attempt completes (or we time out)
		if (timeout >= FOREVER)
			connected_upd_.wait(lock, boost::bind(&data_receiver::connection_completed,this));
		else
			if (!connected_upd_.wait_for(lock, boost::chrono::duration<double>(timeout), boost::bind(&data_receiver::connection_completed,this)))
				throw timeout_error("The open_stream() operation timed out.");
	}
	if (conn_.lost())
		throw lost_error("The stream read by this inlet has been lost. To recover, you need to re-resolve the source and re-create the inlet.");
}

/**
* Close the current data stream.
* All samples still buffered or in flight will be dropped and the source will halt its buffering of data for this inlet.
* If an application stops being interested in data from a source (temporarily or not), it should call drop_stream() to not
* pressure the source outlet to buffer unnecessarily large amounts of data (perhaps even running out of memory).
*/
void data_receiver::close_stream() {
	check_thread_start_ = true;
	closing_stream_ = true;
	cancel_all_registered();
}

/**
* Pull a sample from the inlet and read it into a pointer to raw data.
* No type checking or conversions are done (not recommended!). Do not use for variable-size/string-formatted streams.
* @param buffer A pointer to hold the resulting raw sample data.
* @param buffer_bytes The number of bytes allocated in the buffer. Note: it is the responsibility of the user to allocate enough memory.
* @param block If true, the function will block until a sample is available; otherwise, it will return 0.0 and no data.
* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available.
*		   To remap this time stamp to the local clock, add the value returned by .time_correction() to it.
*		   This is only necessary if the clocks of the source and destination machine are not synchronized to high enough precision.
*/
double data_receiver::pull_sample_untyped(void *buffer, int buffer_bytes, double timeout) {
	if (conn_.lost())
		throw lost_error("The stream read by this inlet has been lost. To recover, you need to re-resolve the source and re-create the inlet.");
	// start data thread implicitly if necessary
	if (check_thread_start_ && !data_thread_.joinable()) {
		data_thread_ = boost::thread(&data_receiver::data_thread,this);
		check_thread_start_ = false;
	}
	// get the sample with timeout
	if (sample_p s = sample_queue_.pop_sample(timeout)) {
		if (buffer_bytes != conn_.type_info().sample_bytes())
			throw std::range_error("The size of the provided buffer does not match the number of bytes in the sample.");
		s->retrieve_untyped(buffer);
		return s->timestamp;
	} else {
		if (conn_.lost())
			throw lost_error("The stream read by this inlet has been lost. To recover, you need to re-resolve the source and re-create the inlet.");
		return 0.0;
	}
}


// === internal processing ===

/// The data reader thread.
void data_receiver::data_thread() {
	conn_.acquire_watchdog();
	// ensure that the sample factory persists for the lifetime of this thread
	sample::factory_p factory(sample_factory_);
	try {
		while (!conn_.lost() && !conn_.shutdown() && !closing_stream_) {
			try {
				// --- connection setup ---

				// make a new stream buffer and a stream on top of it
				boost::asio::cancellable_streambuf<tcp> buffer;
				buffer.register_at(&conn_);
				buffer.register_at(this);
				std::iostream server_stream(&buffer);
				boost::scoped_ptr<eos::portable_iarchive> inarch;
				// connect to endpoint
				buffer.connect(conn_.get_tcp_endpoint());
				if (buffer.puberror())
					throw buffer.puberror();

				// --- protocol negotiation ---

				int use_byte_order = 0;				// which byte order we shall use (0=portable byte order)
				int data_protocol_version = 100;	// which protocol version we shall use for data transmission (100=version 1.00)
				bool suppress_subnormals = false;	// whether we shall suppress subnormal numbers

				// propose to use the highest protocol version supported by both parties
				int proposed_protocol_version = std::min(api_config::get_instance()->use_protocol_version(),conn_.type_info().version());
				if (proposed_protocol_version >= 110) {
					// request line LSL:streamfeed/[ProtocolVersion] [UID]\r\n
					server_stream << "LSL:streamfeed/" << proposed_protocol_version << " " << conn_.current_uid() << "\r\n";
					// transmit request parameters
					server_stream << "Native-Byte-Order: " << BOOST_BYTE_ORDER << "\r\n";
					server_stream << "Endian-Performance: " << std::floor(measure_endian_performance()) << "\r\n";
					server_stream << "Has-IEEE754-Floats: " << (format_ieee754[cf_float32] && format_ieee754[cf_double64]) << "\r\n";
					server_stream << "Supports-Subnormals: " << format_subnormal[conn_.type_info().channel_format()] << "\r\n";
					server_stream << "Value-Size: " << conn_.type_info().channel_bytes() << "\r\n"; // 0 for strings
					server_stream << "Data-Protocol-Version: " << proposed_protocol_version << "\r\n";
					server_stream << "Max-Buffer-Length: " << max_buflen_ << "\r\n";
					server_stream << "Max-Chunk-Length: " << max_chunklen_ << "\r\n";
					server_stream << "Hostname: " << conn_.type_info().hostname() << "\r\n";
					server_stream << "Source-Id: " << conn_.type_info().source_id() << "\r\n";
					server_stream << "Session-Id: " << conn_.type_info().session_id() << "\r\n";
					server_stream << "\r\n" << std::flush;

					// check server response line (LSL/[Version] [StatusCode] [Message])
					char buf[16384] = {0};
					if (!server_stream.getline(buf,sizeof(buf)))
						throw lost_error("Connection lost.");
					std::vector<std::string> parts; split(parts,buf,is_any_of(" \t"));
					if (parts.size() < 3 || !starts_with(parts[0],"LSL/"))
						throw std::runtime_error("Received a malformed response.");
					if (boost::lexical_cast<int>(parts[0].substr(4))/100 > api_config::get_instance()->use_protocol_version()/100)
						throw std::runtime_error("The other party's protocol version is too new for this client; please upgrade your LSL library.");
					int status_code = boost::lexical_cast<int>(parts[1]);
					if (status_code == 404)
						throw lost_error("The given address does not serve the resolved stream (likely outdated).");
					if (status_code >= 400)
						throw std::runtime_error("The other party sent an error: " + std::string(buf));
					if (status_code >= 300)
						throw lost_error("The other party requested a redirect.");

					// receive response parameters
					while (server_stream.getline(buf,sizeof(buf)) && (buf[0] != '\r')) {
						std::string hdrline(buf);
						std::size_t colon = hdrline.find_first_of(':');
						if (colon != std::string::npos) {
							// extract key & value
							std::string type = to_lower_copy(trim_copy(hdrline.substr(0,colon))), rest = to_lower_copy(trim_copy(hdrline.substr(colon+1)));
							// strip off comments
							std::size_t semicolon = rest.find_first_of(';');
							if (semicolon != std::string::npos)
								rest = rest.substr(0,semicolon);
							// get the header information
							if (type == "byte-order") {
								use_byte_order = boost::lexical_cast<int>(rest);
								if (use_byte_order==2134 && BOOST_BYTE_ORDER!=2134 && format_sizes[conn_.type_info().channel_format()]>=8)
									throw std::runtime_error("The byte order conversion requested by the other party is not supported.");
							}
							if (type == "suppress-subnormals") 
								suppress_subnormals = boost::lexical_cast<bool>(rest);
							if (type == "uid" && rest != conn_.current_uid())
								throw lost_error("The received UID does not match the current connection's UID.");
							if (type == "data-protocol-version") {
								data_protocol_version = boost::lexical_cast<int>(rest);
								if (data_protocol_version > api_config::get_instance()->use_protocol_version())
									throw std::runtime_error("The protocol version requested by the other party is not supported by this client.");
							}
						}
					}
					if (!server_stream)
						throw lost_error("Server connection lost.");
				} else {
					// version 1.00: send request line and feed parameters
					server_stream << "LSL:streamfeed\r\n";
					server_stream << max_buflen_ << " " << max_chunklen_ << "\r\n" << std::flush;
				}

				if (data_protocol_version == 100) {
					// portable binary archive (parse archive header)
					inarch.reset(new eos::portable_iarchive(server_stream));
					// receive stream_info message from server
					std::string infomsg; *inarch >> infomsg;
					stream_info_impl info; info.from_shortinfo_message(infomsg);
					// confirm that the UID matches, otherwise reconnect
					if (info.uid() != conn_.current_uid())
						throw lost_error("The received UID does not match the current connection's UID.");
				}

				// --- format validation ---
				{
					// receive and parse two subsequent test-pattern samples and check if they are formatted as expected
					boost::scoped_ptr<sample> temp[4]; 
					for (int k=0; k<4; temp[k++].reset(sample::factory::new_sample_unmanaged(conn_.type_info().channel_format(),conn_.type_info().channel_count(),0.0,false)));
					temp[0]->assign_test_pattern(4); if (data_protocol_version >= 110) temp[1]->load_streambuf(buffer,data_protocol_version,use_byte_order,suppress_subnormals); else *inarch >> *temp[1];
					temp[2]->assign_test_pattern(2); if (data_protocol_version >= 110) temp[3]->load_streambuf(buffer,data_protocol_version,use_byte_order,suppress_subnormals); else *inarch >> *temp[3];
					if (!(*temp[0].get() == *temp[1].get()) || !(*temp[2].get() == *temp[3].get()))
						throw std::runtime_error("The received test-pattern samples do not match the specification. The protocol formats are likely incompatible.");
				}

                // signal to accessor functions on other threads that the protocol negotiation has been successful,
                // so we're now connected (and remain to be even if we later recover silently)
                {
                    boost::lock_guard<boost::mutex> lock(connected_mut_);
                    connected_ = true;
                }
                connected_upd_.notify_all();

                // --- transmission loop ---

                double last_timestamp = 0.0;
                double srate = conn_.current_srate();
                for (int k=0;!conn_.lost() && !conn_.shutdown() && !closing_stream_;k++) {
                    // allocate and fetch a new sample						
                    sample_p samp(factory->new_sample(0.0,false));
					if (data_protocol_version >= 110) samp->load_streambuf(buffer,data_protocol_version,use_byte_order,suppress_subnormals); else *inarch >> *samp;
                    // deduce timestamp if necessary
                    if (samp->timestamp == DEDUCED_TIMESTAMP) {
                        samp->timestamp = last_timestamp;
                        if (srate != IRREGULAR_RATE)
                            samp->timestamp += 1.0/srate;
                    }
                    last_timestamp = samp->timestamp;
                    // push it into the sample queue
                    sample_queue_.push_sample(samp);
                    // periodically update the last receive time to keep the watchdog happy
					if (srate<=16 || (k & 0xF) == 0)
						conn_.update_receive_time(lsl_clock());
                }
			}
			catch(error_code &) {
				// connection-level error: closed, reset, refused, etc.
				conn_.try_recover_from_error();
			}
			catch(lost_error &) {
				// another type of connection error
				conn_.try_recover_from_error();
			}
			catch(shutdown_error &) {
				// termination due to connection shutdown
				throw lost_error("The inlet has been disengaged.");
			}
			catch(std::exception &e) {
				// some perhaps more serious transmission or parsing error (could be indicative of a protocol issue)
				if (!conn_.shutdown())
					std::cerr << "Stream transmission broke off (" << e.what() << "); re-connecting..." << std::endl;
				conn_.try_recover_from_error();
			}
            // wait for a few msec so as to not spam the provider with reconnects
            boost::this_thread::sleep(boost::posix_time::millisec(500));
		}
	}
	catch(lost_error &) {
		// the connection was irrecoverably lost: since the pull_sample() function may
		// be waiting for the next sample we need to wake it up by passing a sentinel
		sample_queue_.push_sample(sample_p());
	}
	conn_.release_watchdog();
}

