
#include <iostream>
#include "main.h"



void send_buffer::push_sample(sample_p samp) {
	boost::lock_guard<boost::mutex> lock(mut);
	for (queue_register::iterator i = queues_.begin(); i != queues_.end(); i++)
		(*i)->push_sample(samp);
}

void send_buffer::register_queue(queue *q) {
	boost::lock_guard<boost::mutex> lock(mut);
	queues_.insert(q);
}

queue_p send_buffer::new_queue(int size) {
	return queue_p(new queue(100, shared_from_this()));
}

// queue definitions
queue::queue(int size, const send_buffer_p &send_buffer) : buffer_(size), send_buffer_(send_buffer) {
	send_buffer_->register_queue(this);
}

void queue::push_sample(const sample_p &samp) {

	while (!buffer_.push(samp)) {
		sample_p dummy;
		buffer_.pop(dummy);
	}
}

sample_p queue::pop_sample(void) {
	sample_p result;
	do {
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
	} while (!buffer_.pop(result));
	return result;
}

// server defenitions
server::server(const send_buffer_p &send_buffer) : send_buffer_(send_buffer), factory_(new sample::factory()){
	

}


void server::write_samples() {

	for (int i = 0; i < 100; i++)
	{
		sample_p s = factory_->new_sample(100, (double) i);
		send_buffer_->push_sample(s);
	}

}

// client definitions
client::client(const server_p &serv) : serv_(serv){}

bool keep_clienting;
void client::read_samples(void) {

	queue_p q = serv_->send_buffer_->new_queue(100);
	while(keep_clienting) {
		sample_p samp = q->pop_sample();
		std::cout << samp->data << std::endl;
	}

}

int main(void) {
	keep_clienting = true;

	send_buffer_p the_send_buffer(new send_buffer());
	queue_p the_queue(new queue(100, the_send_buffer));
	server_p the_server(new server(the_send_buffer));
	client the_client(the_server);

	boost::thread t1(boost::bind(&client::read_samples, &the_client));
	std::cout << "enter a to send 100 samples, b to exit" << std::endl;
	char foo = 'x';
	while (foo != 'b') {
		std::cin >> foo;
		if (foo == 'a')
			the_server->write_samples();
		if (foo == 'b') {
			keep_clienting = false;
			// send one last sample to continue through the receive loop
			the_server->send_buffer_->push_sample(the_server->factory_->new_sample(1, 0.0));
			break;
		}
	}
		
	t1.join();

	return 0;
}