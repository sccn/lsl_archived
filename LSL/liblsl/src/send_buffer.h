#ifndef SEND_BUFFER_H
#define SEND_BUFFER_H

#include <set>
#include <boost/container/flat_set.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "consumer_queue.h"
#include "sample.h"



namespace lsl {

	/**
	* A thread-safe single-producer multiple-consumer queue where each consumer gets every pushed sample.
	* If the bounded capacity is exhausted, the oldest samples will be erased.
	* 
	* Note that the send_buffer is actually just a dispatcher that distributes the data to producer-consumer 
	* queues (each of which can have its own capacity preferences). The ownership of the send_buffer is shared 
	* between the consumer_queues and the owner of the send_buffer.
	*/
	class send_buffer: public boost::enable_shared_from_this<send_buffer> {
		typedef boost::container::flat_set<consumer_queue*> consumer_set;
	public:
		/**
		* Create a new send buffer.
		* @param max_capacity Hard upper bound on queue capacity beyond which the oldest samples will be dropped.
		*/
		send_buffer(int max_capacity);

		/**
		* Add a new consumer queue to the buffer.
		* Each consumer will get all samples (although the oldest samples will be dropped when the buffer capacity is overrun).
		* The consumer is automatically removed upon destruction.
		* @param max_buffered If non-zero, the queue size for this consumer will be constrained to be no larger than this value.
		*					  Note that the actual queue size will never exceed the max_capacity of the send_buffer (so this is 
		*					  a global limit).
		* @return Shared pointer to the newly created consumer.
		*/
		consumer_queue_p new_consumer(int max_buffered=0);

		/** 
		* Push a sample onto the send buffer. 
		* Will subsequently be received by all consumers.
		*/
		void push_sample(const sample_p &s);

		/// Wait until some consumers are present.
		bool wait_for_consumers(double timeout=FOREVER);

		/// Check whether any consumer is currently registered.
		bool have_consumers();

	private:
		friend class consumer_queue;

		/// Registered a new consumer (called by the consumer_queue).
		void register_consumer(consumer_queue *q);
		/// Unregister a previously registered consumer (called by the consumer_queue).
		void unregister_consumer(consumer_queue *q);

		/// wait_for_consumers is waiting for this
		bool some_registered() const { return !consumers_.empty(); }

		int max_capacity_;							// maximum capacity beyond which the oldest samples will be dropped
		consumer_set consumers_;					// a set of registered consumer queues
		boost::mutex consumers_mut_;				// mutex to protect the integrity of consumers_
		boost::condition_variable some_registered_;	// condition variable signaling that a consumer has registered
	};

}

#endif

