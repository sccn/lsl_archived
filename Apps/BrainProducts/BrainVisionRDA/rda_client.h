#ifndef RDACLIENT_H
#define RDACLIENT_H


#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "rda_message.h"

using boost::asio::ip::tcp;



class RDA_message_queue
{
public:
	void push_back(const RDA_message &data)
	{
		boost::lock_guard<boost::mutex> l(m_);
		q_.push_back(data);
	}

	RDA_message& front()
	{
		boost::lock_guard<boost::mutex> l(m_);
		RDA_message& f = q_.front();
		return f;
	}

	void pop_front()
	{
		boost::lock_guard<boost::mutex> l(m_);
		q_.pop_front();
	}

	bool empty()
	{
		boost::lock_guard<boost::mutex> l(m_);
		return q_.empty();
	}


private:
	boost::mutex m_; 
	std::deque<RDA_message> q_;
};

//typedef std::deque<RDA_message> RDA_message_queue;

class RDA_client
{
public:
	RDA_client(boost::asio::io_service& io_service,
		tcp::socket& socket) :
		io_service_(io_service),
		socket_(socket),
		is_connected_(true)
	{
		// start reading message header from socket 
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.header(), RDA_message::header_length),
			boost::bind(&RDA_client::handle_read_header, this,
			boost::asio::placeholders::error));
		// start a new io thread
		io_thread_.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_, ec_)));
	}

	~RDA_client()
	{
		if(io_thread_->joinable())
			io_thread_->join();
	}

	bool is_connected()
	{
		return is_connected_;
	}

	RDA_message_queue& messages(void)
	{
		return read_msgs_;
	}

private:
	void handle_read_header(const boost::system::error_code& error)
	{
		if (!error && read_msg_.decode_header())
		{
			// start reading message body from socket 
			boost::asio::async_read(socket_,
				boost::asio::buffer(read_msg_.body()),
				//boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
				boost::bind(&RDA_client::handle_read_body, this,
				boost::asio::placeholders::error));
		}
		else
		{
			do_close();
		}
	}

	void handle_read_body(const boost::system::error_code& error)
	{
		if (!error)
		{
			// put message into message queue
			read_msgs_.push_back(read_msg_);
			// start reading next message header from socket 
			boost::asio::async_read(socket_,
				boost::asio::buffer(read_msg_.header(), RDA_message::header_length),
				boost::bind(&RDA_client::handle_read_header, this,
				boost::asio::placeholders::error));
		}
		else
		{
		  do_close();
		}
	}

	void do_close()
	{
		boost::system::error_code ec;
		socket_.close(ec);
		is_connected_ = false;
	}

private:
	boost::asio::io_service& io_service_;
	tcp::socket& socket_;
	RDA_message read_msg_;
	RDA_message_queue read_msgs_;
	boost::shared_ptr<boost::thread> io_thread_;	// our io thread
	bool is_connected_;
	boost::system::error_code ec_;
};



#endif // RDACLIENT_H
