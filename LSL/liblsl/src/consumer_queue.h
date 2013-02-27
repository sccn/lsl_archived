#ifndef CONSUMER_QUEUE_H
#define CONSUMER_QUEUE_H

#include "sample.h"
#include BOOST_PATH(/circular_buffer.hpp)
#include BOOST_PATH(/thread.hpp)

namespace lsl {
	/// shared pointer to a consumer queue
	typedef boost::shared_ptr<class consumer_queue> consumer_queue_p;
	/// shared pointer to a send buffer
	typedef boost::shared_ptr<class send_buffer> send_buffer_p;

	/**
	* A thread-safe producer-consumer queue of unread samples.
	* Erases the oldest samples if max capacity is exceeded. Implemented as a circular buffer.
	*/
	class consumer_queue: private boost::noncopyable {
		typedef boost::circular_buffer_space_optimized<sample_p> buffer_type;
	public:
		/**
		* Create a new queue with a given capacity.
		* @param max_capacity The maximum number of samples that can be held by the queue. Beyond that, the oldest samples are dropped.
		* @param min_capacity The minimum capacity reserved by the queue (the queue does not try to shrink below this).
		* @param registry Optionally a pointer to a registration facility, for multiple-reader arrangements.
		*/
		consumer_queue(std::size_t max_capacity, std::size_t min_capacity, send_buffer_p registry=send_buffer_p());

		/**
		* Destructor.
		* Unregisters from the send buffer, if any.
		*/
		~consumer_queue();

		/**
		* Push a new sample onto the queue.
		*/
		void push_sample(const sample_p &sample);

		/**
		* Pop a sample from the queue. 
		* Blocks if empty.
		* @param timeout Timeout for the blocking, in seconds. If expired, an empty sample is returned.
		*/
		sample_p pop_sample(double timeout=FOREVER);

		/**
		* Query the current size of the buffer, i.e. the number of samples that are buffered.
		*/ 
		std::size_t buffer_size();

	private:
		/// pop_back is waiting for this condition
		bool is_not_empty() const { return !buffer_.empty(); }

		send_buffer_p registry_;				// optional consumer registry
		buffer_type buffer_;					// the sample buffer
		boost::mutex buffer_mut_;				// mutex to protect the buffer's integrity
		boost::condition_variable not_empty_;	// condition variable signaling that the buffer is not empty
	};

}

#endif

