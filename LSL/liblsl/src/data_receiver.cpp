#include "data_receiver.h"
#include "portable_archive/portable_iarchive.hpp"
#include <iostream>
#include <boost/serialization/string.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/bind.hpp>


// === implementation of the data_receiver class ===

using namespace lsl;

/**
* Construct a new data receiver from an info connection.
* @param conn An inlet connection object.
* @param max_buflen Optionally the maximum amount of data to buffer (in seconds if there is a nominal sampling rate, otherwise x 100 in samples).
*					Recording applications want to use a fairly large buffer size here, while real-time applications want to only buffer as much as they need to perform their next calculation.
* @param max_chunklen Optionally the maximum size, in samples, at which chunks are transmitted (the default corresponds to the chunk sizes used by the sender).
*					  Recording applications can use a generous size here (leaving it to the network how to pack things), while real-time applications may want a finer (perhaps 1-sample) granularity.
*/
data_receiver::data_receiver(inlet_connection &conn, int max_buflen, int max_chunklen): conn_(conn), closing_stream_(false), connected_(false), sample_queue_(max_buflen,32768), max_buflen_(max_buflen), max_chunklen_(max_chunklen) {
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
		if (!data_thread_.joinable())
			data_thread_ = boost::thread(&data_receiver::data_thread,this);
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
	if (!data_thread_.joinable())
		data_thread_ = boost::thread(&data_receiver::data_thread,this);
	// get the sample with timeout
	if (sample_p s = sample_queue_.pop_sample(timeout)) {
		if (buffer_bytes != conn_.type_info().sample_bytes())
			throw std::range_error("The size of the provided buffer does not match the number of bytes in the sample.");
		s->retrieve_untyped_ptr(buffer);
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
	try {
		while (!conn_.lost() && !conn_.shutdown() && !closing_stream_) {
			try {
				// --- connection setup ---

				// make a new stream buffer and a stream on top of it
				boost::asio::cancellable_streambuf<tcp> buffer;
				buffer.register_at(&conn_);
				buffer.register_at(this);
				std::iostream server_stream(&buffer);
				// connect to endpoint
				buffer.connect(conn_.get_tcp_endpoint());
				if (buffer.puberror())
					throw buffer.puberror();

				// --- protocol negotiation ---

				// send the query
				server_stream << "LSL:streamfeed\r\n" << std::flush;
				server_stream << max_buflen_ << " " << max_chunklen_ << "\r\n" << std::flush;
				// parse the info & check whether it matches
				eos::portable_iarchive inarch_(server_stream);
				std::string infomsg; inarch_ >> infomsg;
				stream_info_impl info; info.from_shortinfo_message(infomsg);
				// if the stream has not changed since we established the connection...
				if (info.uid() == conn_.current_uid()) {
                    // parse the subsequent two test-pattern samples and check if they are formatted as expected
                    sample samp1(conn_.type_info(),0.0,false), samp2(conn_.type_info(),0.0,false), proof1(conn_.type_info(),0.0,false), proof2(conn_.type_info(),0.0,false);
                    proof1.assign_test_pattern(4);
                    proof2.assign_test_pattern(2);
                    inarch_ >> samp1 >> samp2;
                    if (!(samp1 == proof1) || !(samp2 == proof2))
                        throw std::runtime_error("The received test-pattern samples do not match the specification. The protocol versions are likely incompatible.");

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
                    while (!conn_.lost() && !conn_.shutdown() && !closing_stream_) {
                        // allocate and fetch a new sample
                        sample_p samp(new sample(conn_.type_info(),0.0,false));
                        inarch_ >> *samp;
                        // deduce timestamp if necessary
                        if (samp->timestamp == DEDUCED_TIMESTAMP) {
                            if (srate == IRREGULAR_RATE)
                                samp->timestamp = last_timestamp;
                            else
                                samp->timestamp = last_timestamp + 1.0/srate;
                        }
                        last_timestamp = samp->timestamp;
                        // push it into the sample queue
                        sample_queue_.push_sample(samp);
                        // update the last receive time to keep the watchdog happy
                        conn_.update_receive_time(local_clock());
                    }
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
			catch(std::exception &) {
				// some perhaps more serious transmission or parsing error (could be indicative of a protocol issue)
				if (!conn_.shutdown())
					std::cerr << "Stream transmission broke off; re-connecting..." << std::endl;
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

