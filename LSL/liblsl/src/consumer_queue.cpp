#include "consumer_queue.h"
#include "send_buffer.h"
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <iostream>

//
// === implementation of the consumer_queue class ===
//

using namespace lsl;

/**
* Create a new queue with a given capacity.
* @param max_capacity The maximum number of samples that can be held by the queue. Beyond that, the oldest samples are dropped.
* @param min_capacity The minimum capacity reserved by the queue (the queue does not try to shrink below this).
* @param registry Optionally a pointer to a registration facility, to dispatch samples to all consumers.
*/
consumer_queue::consumer_queue(std::size_t max_capacity, std::size_t min_capacity, send_buffer_p registry): registry_(registry), buffer_(buffer_type::capacity_type(max_capacity,std::min(max_capacity,min_capacity)))  {
	if (registry_)
		registry_->register_consumer(this);
}

/**
* Destructor.
* Unregisters from the send buffer, if any.
*/
consumer_queue::~consumer_queue() {
	try {
		if (registry_)
			registry_->unregister_consumer(this);
	} catch(std::exception &e) {
		std::cerr << "Unexpected error while trying to unregister a consumer queue from its registry:" << e.what() << std::endl;
	}
}

/**
* Push a new sample onto the queue.
*/
void consumer_queue::push_sample(const sample_p &sample) {
	{
		boost::lock_guard<boost::mutex> lock(buffer_mut_);
		buffer_.push_back(sample);
	}
	not_empty_.notify_all();
}

/**
* Pop a sample from the queue.
* Blocks if empty.
* @param timeout Timeout for the blocking, in seconds. If expired, an empty sample is returned.
*/
sample_p consumer_queue::pop_sample(double timeout) {
	boost::unique_lock<boost::mutex> lock(buffer_mut_);
	if (timeout >= FOREVER) {
		not_empty_.wait(lock, boost::bind(&consumer_queue::is_not_empty,this));
		sample_p result = buffer_.front();
		buffer_.pop_front();
		return result;
	} else
		if (timeout <= 0.0) {
			if (is_not_empty()) {
				sample_p result = buffer_.front();
				buffer_.pop_front();
				return result;
			} else
				return sample_p();
		} else {
			if (not_empty_.wait_for(lock, boost::chrono::duration<double>(timeout), boost::bind(&consumer_queue::is_not_empty,this))) {
				sample_p result = buffer_.front();
				buffer_.pop_front();
				return result;
			} else
				return sample_p();
		}
}

std::size_t consumer_queue::buffer_size() {
	boost::lock_guard<boost::mutex> lock(buffer_mut_);
	return buffer_.size();
}

