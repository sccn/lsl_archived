#pragma once
#include <set>
#include <boost/container/flat_set.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

typedef boost::intrusive_ptr<class sample> sample_p;
typedef boost::lockfree::spsc_queue<sample_p> buffer;
typedef boost::shared_ptr<class queue> queue_p;
typedef boost::container::flat_set<queue*> queue_register;
typedef boost::shared_ptr<class server> server_p;
typedef boost::shared_ptr<class send_buffer> send_buffer_p;


class sample {

public:
	double data;
	class factory;
	typedef boost::shared_ptr<class factory> factory_p;
	class factory {
	public:
		friend class sample;
		sample_p new_sample(int size, double data) {
			sample* result = new(new char[size]) sample(data);
			return sample_p(result);
		}
	};

	sample(double d) : ref_count(0){
		data = d;
		
	}

	void operator delete(void *x) {
		delete[](char*)x;
	}

	boost::atomic<int> ref_count;
	/// Increment ref count.
	friend void intrusive_ptr_add_ref(sample *s) {
		s->ref_count.fetch_add(1, boost::memory_order_relaxed);
	}

	/// Decrement ref count and reclaim if unreferenced.
	friend void intrusive_ptr_release(sample *s) {
		if (s->ref_count.fetch_sub(1, boost::memory_order_release) == 1)
			// do nothing and leak memory
			;
			// free memory and cause problems
			//delete[](char*)s;
	}

};

class send_buffer : public boost::enable_shared_from_this<send_buffer>{
public:
	queue_register queues_;
	boost::mutex mut;
	void push_sample(sample_p samp);
	void register_queue(queue *q);
	queue_p send_buffer::new_queue(int size);
};

class queue {
public:
	buffer buffer_;
	send_buffer_p send_buffer_;
	queue(int size, const send_buffer_p &send_buffer);
	void push_sample(const sample_p &samp);
	sample_p pop_sample(void);
};

class server : public boost::enable_shared_from_this<server> {
public:
	send_buffer_p send_buffer_;
	sample::factory_p factory_;
	server(const send_buffer_p &send_buffer);
	void write_samples();
};

class client : public boost::enable_shared_from_this<client>{
public:
	server_p serv_;
	client(const server_p &serv);
	void read_samples(void);
};
