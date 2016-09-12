#include "time_receiver.h"
#include "api_config.h"
#include <iostream>
#include <boost/bind.hpp>


// === implementation of the time_receiver class ===

using namespace lsl;
using namespace boost::asio;
using boost::posix_time::millisec;

/**
* Construct a new time provider from an inlet connection
*/
time_receiver::time_receiver(inlet_connection &conn): conn_(conn), timeoffset_(std::numeric_limits<double>::max()),
       remote_time_(std::numeric_limits<double>::max()), uncertainty_(std::numeric_limits<double>::max()), was_reset_(false),
	   cfg_(api_config::get_instance()), time_sock_(time_io_), next_estimate_(time_io_), aggregate_results_(time_io_), next_packet_(time_io_) {
	conn_.register_onlost(this,&timeoffset_upd_);
	conn_.register_onrecover(this,boost::bind(&time_receiver::reset_timeoffset_on_recovery,this));
	time_sock_.open(conn_.udp_protocol());
}

/// Destructor. Stops the background activities.
time_receiver::~time_receiver() {
	try {
		conn_.unregister_onrecover(this);
		conn_.unregister_onlost(this);
		time_io_.stop();
		if (time_thread_.joinable())
			time_thread_.join();
	} 
	catch(std::exception &e) {
		std::cerr << "Unexpected error during destruction of a time_receiver: " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "Severe error during time receiver shutdown." << std::endl;
	}
}


// === external data acccess ===

/**
* Retrieve an estimated time correction offset for the given stream.
* The first call to this function takes several msec for an initial estimate, subsequent calls are instantaneous.
* The correction offset is periodically re-estimated in the background (once every few sec.).
* @remote_time Time of this measurment on remote computer
* @uncertainty Maximum uncertainty of this measurement (maps to round-trip-time).
* @timeout Timeout for first time-correction estimate.
* @return The time correction estimate.
* @throws timeout_error If the initial estimate times out.
*/

double time_receiver::time_correction(double timeout) {
    double remote_time, uncertainty;
    return time_correction(&remote_time, &uncertainty, timeout);

}

double time_receiver::time_correction(double *remote_time, double *uncertainty, double timeout ) {
	boost::unique_lock<boost::mutex> lock(timeoffset_mut_);
	if (!timeoffset_available()) {
		// start thread if not yet running
		if (!time_thread_.joinable())
			time_thread_ = boost::thread(&time_receiver::time_thread,this);
		// wait until the timeoffset becomes available (or we time out)
		if (timeout >= FOREVER)
			timeoffset_upd_.wait(lock, boost::bind(&time_receiver::timeoffset_available,this));
		else
			if (!timeoffset_upd_.wait_for(lock, boost::chrono::duration<double>(timeout), boost::bind(&time_receiver::timeoffset_available,this)))
				throw timeout_error("The time_correction() operation timed out.");
	}
	if (conn_.lost())
		throw lost_error("The stream read by this inlet has been lost. To recover, you need to re-resolve the source and re-create the inlet.");
	*remote_time = remote_time_;
	*uncertainty = uncertainty_;
	return timeoffset_;
}

/// Determine whether the clock was (potentially) reset since the last call to was_reset()
/// This can happen if the stream got lost (e.g., app crash) and the computer got restarted or swapped out
bool time_receiver::was_reset() {
	boost::unique_lock<boost::mutex> lock(timeoffset_mut_);
	bool result = was_reset_;
	was_reset_ = false;
	return result;
}

// === internal processing ===

/// The time reader & updater thread.
void time_receiver::time_thread() {
	conn_.acquire_watchdog();
	try {
		// start an async time estimation
		start_time_estimation();
		// start the IO object (will keep running until cancelled)
		while (true) {
			try {
				time_io_.run();
				break;
			} catch(std::exception &e) {
				std::cerr << "Hiccup during time_thread io_service processing: " << e.what() << std::endl;
			}
		}
	} catch(std::exception &e) {
		std::cerr << "time_thread failed unexpectedly with message: " << e.what() << std::endl;
	}
	conn_.release_watchdog();
}

/// Start a new multi-packet exchange for time estimation
void time_receiver::start_time_estimation() {
	// clear the estimates buffer
	estimates_.clear();
	estimate_times_.clear();
	// generate a new wave id so that we don't confuse packets from earlier (or mis-guided) estimations
	current_wave_id_ = rng_();
	// start the packet exchange chains
	send_next_packet(1);
	receive_next_packet();
	// schedule the aggregation of results (by the time when all replies should have been received)
	aggregate_results_.expires_from_now(millisec(1000*(cfg_->time_probe_max_rtt() + cfg_->time_probe_interval()*cfg_->time_probe_count())));
	aggregate_results_.async_wait(boost::bind(&time_receiver::result_aggregation_scheduled,this,placeholders::error));
	// schedule the next estimation step
	next_estimate_.expires_from_now(millisec(1000*cfg_->time_update_interval()));
	next_estimate_.async_wait(boost::bind(&time_receiver::next_estimate_scheduled,this,placeholders::error));
}

/// Handler that gets called once the next time estimation shall be scheduled
void time_receiver::next_estimate_scheduled(error_code err) {
	if (err != error::operation_aborted)
		start_time_estimation();
}

/// Send the next packet in an exchange
void time_receiver::send_next_packet(int packet_num) {
	try {
		// form the request & send it
		std::ostringstream request; request.precision(16); request << "LSL:timedata\r\n" << current_wave_id_ << " " << lsl_clock() << "\r\n";
		string_p msg_buffer(new std::string(request.str()));
		time_sock_.async_send_to(boost::asio::buffer(*msg_buffer), conn_.get_udp_endpoint(),
			boost::bind(&time_receiver::handle_send_outcome,this,msg_buffer,placeholders::error));
	} catch(std::exception &e) {
		std::cerr << "Error trying to send a time packet: " << e.what() << std::endl;
	}
	// schedule next packet
	if (packet_num < cfg_->time_probe_count()) {
		next_packet_.expires_from_now(millisec(1000.0*cfg_->time_probe_interval()));
		next_packet_.async_wait(boost::bind(&time_receiver::next_packet_scheduled,this,++packet_num,placeholders::error));
	}
}

/// Handler that gets called once the sending of a packet has completed
void time_receiver::handle_send_outcome(string_p, error_code) { }

/// Handler that gets called when the next packet shall be scheduled
void time_receiver::next_packet_scheduled(int packet_num, error_code err) {
	if (!err)
		send_next_packet(packet_num);
}

/// Request reception of the next time packet
void time_receiver::receive_next_packet() {
	time_sock_.async_receive_from(boost::asio::buffer(recv_buffer_), remote_endpoint_,
		boost::bind(&time_receiver::handle_receive_outcome, this, placeholders::error, placeholders::bytes_transferred));
}

/// Handler that gets called once reception of a time packet has completed
void time_receiver::handle_receive_outcome(error_code err, std::size_t len) {
	try {
		if (!err) {
			// parse the buffer contents
			std::istringstream is(std::string(recv_buffer_,len));
			int wave_id; is >> wave_id;
			if (wave_id == current_wave_id_) {
				double t0, t1, t2, t3 = lsl_clock();
				is >> t0 >> t1 >> t2;
				// calculate RTT and offset
				double rtt = (t3-t0) - (t2-t1);				// round trip time (time passed here - time passed there)
				double offset = ((t1-t0) + (t2-t3)) / 2;	// averaged clock offset (other clock - my clock) with rtt bias averaged out
				// store it
				estimates_.push_back(std::make_pair(rtt,offset));
				estimate_times_.push_back(std::make_pair((t3 + t0)/2.0, (t2 + t1)/2.0));   //local_time, remote_time
			}
		}
	} catch(std::exception &e) {
		std::cerr << "Error while processing a time estimation return packet: " << e.what() << std::endl;
	}
	if (err != error::operation_aborted)
		receive_next_packet();
}

/// Handlers that gets called once the time estimation results shall be aggregated.
void time_receiver::result_aggregation_scheduled(error_code err) {
	if (!err) {
		if ((int)estimates_.size() >= cfg_->time_update_minprobes()) {
			// take the estimate with the lowest error bound (=rtt), as in NTP
			double best_offset=0, best_rtt=FOREVER;
			double best_remote_time=0;
			for (unsigned k=0;k<estimates_.size();k++) {
				if (estimates_[k].first < best_rtt) {
					best_rtt = estimates_[k].first;
					best_offset = estimates_[k].second;
					best_remote_time = estimate_times_[k].second;
				}
			}
			// and notify that the result is available
			{
				boost::lock_guard<boost::mutex> lock(timeoffset_mut_);
                uncertainty_ = best_rtt;
				timeoffset_ = -best_offset;
				remote_time_ = best_remote_time;
			}
			timeoffset_upd_.notify_all();
		}
	}
}

/// Ensures that the time-offset is reset when the underlying connection is recovered (e.g., switches to another host)
void time_receiver::reset_timeoffset_on_recovery() {
	boost::lock_guard<boost::mutex> lock(timeoffset_mut_);
	if (timeoffset_ != NOT_ASSIGNED)
		// this will only be set to true if the reset may have caused a possible interruption in the obtained time offsets
		was_reset_ = true;
	timeoffset_ = NOT_ASSIGNED;
}
