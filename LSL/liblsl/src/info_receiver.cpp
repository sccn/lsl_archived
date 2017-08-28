#include "info_receiver.h"
#include "cancellable_streambuf.h"
#include <iostream>
#include <boost/bind.hpp>


// === implementation of the info_receiver class ===

using namespace lsl;

/// Construct a new info receiver.
info_receiver::info_receiver(inlet_connection &conn): conn_(conn) {
	conn_.register_onlost(this,&fullinfo_upd_);
}

/// Destructor. Stops the background activities.
info_receiver::~info_receiver() {
	try {
		conn_.unregister_onlost(this);
		if (info_thread_.joinable())
			info_thread_.join();
	} 
	catch(std::exception &e) {
		std::cerr << "Unexpected error during destruction of an info_receiver: " << e.what() << std::endl;
	}
	catch(...) {
		std::cerr << "Severe error during info receiver shutdown." << std::endl;
	}
}

/// Retrieve the complete information of the given stream, including the extended description.
const stream_info_impl &info_receiver::info(double timeout) {
	boost::unique_lock<boost::mutex> lock(fullinfo_mut_);
	if (!info_ready()) {
		// start thread if not yet running
		if (!info_thread_.joinable())
			info_thread_ = boost::thread(&info_receiver::info_thread,this);
		// wait until we are ready to return a result (or we time out)
		if (timeout >= FOREVER)
			fullinfo_upd_.wait(lock, boost::bind(&info_receiver::info_ready,this));
		else
			if (!fullinfo_upd_.wait_for(lock, boost::chrono::duration<double>(timeout), boost::bind(&info_receiver::info_ready,this)))
				throw timeout_error("The info() operation timed out.");
	}
	if (conn_.lost())
		throw lost_error("The stream read by this inlet has been lost. To recover, you need to re-resolve the source and re-create the inlet.");
	return *fullinfo_;
}

/// The info reader thread.
void info_receiver::info_thread() {
	conn_.acquire_watchdog();
	try {
		while (!conn_.lost() && !conn_.shutdown()) {
			try {
				// make a new stream buffer & stream
				boost::asio::cancellable_streambuf<tcp> buffer;
				buffer.register_at(&conn_);
				std::iostream server_stream(&buffer);
				// connect...
				buffer.connect(conn_.get_tcp_endpoint());
				// send the query
				server_stream << "LSL:fullinfo\r\n" << std::flush;
				// receive and parse the response
				std::ostringstream os; os << server_stream.rdbuf();
				stream_info_impl info;
				std::string msg = os.str();
				info.from_fullinfo_message(msg);
				// if this is not a valid streaminfo we retry
				if (!info.created_at())
					continue;
				// store the result for pickup & return
				{
					boost::lock_guard<boost::mutex> lock(fullinfo_mut_);
					fullinfo_ = stream_info_impl_p(new stream_info_impl(info));
				}
				fullinfo_upd_.notify_all();
				break;
			}
			catch(error_code &) {
				// connection-level error: closed, reset, refused, etc.
				conn_.try_recover_from_error();
			}
			catch(std::exception &e) {
				// parsing-level error: intermittent disconnect or invalid protocol
				std::cerr << "Error while receiving the stream info (" << e.what() << "); retrying..." << std::endl;
				conn_.try_recover_from_error();
			}
		}
	} catch(lost_error &) { }
	conn_.release_watchdog();
}

