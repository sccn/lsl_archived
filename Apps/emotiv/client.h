#ifndef PUPILPROCLIENT_H
#define PUPILPROCLIENT_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>


using boost::asio::ip::tcp;
boost::uuids::uuid UUID = { 0x8E, 0x45, 0x58, 0x43, 0x96, 0xC9, 0x86, 0x4C, 0xAF, 0x4A, 0x98, 0xBB, 0xF6, 0xC9, 0x14, 0x50};


// we create a generic header type, leaving the body type to be customized
struct TCP_Header
{
	uint8_t uuid[16];				// message uuid (RDA_UUID)
	uint32_t size;					// Size of the message block in bytes including this header
	uint32_t type;					// Message type.
};

class TCP_Message
{

// here is the generic TCP_Message object
// the body object will use this code to get the message data in the form of
// a uint8_t vector -- then the custom object must decode as necessary
public:
	enum{header_length = sizeof(TCP_MessageHeader)};

	TCP_Message():body_length_(0){}

	bool decode_header()
	{
		body_length_ = header_.size - sizeof(TCP_MessageHeader);
		data_.resize(body_length_);
		boost::uuids::uuid u;
		memcpy(&u, header_.uuid, 16);
		if(UUID == u)
			return true;
		else
			return false;
	}

	// utility public member functions
	size_t length() const
	{
		return header_length + body_length_;
	}

	void* header()
	{
		return (void*)&header_;
	}

	const std::vector<uint8_t>& body() const
	{
		return data_;
	}

	std::vector<uint8_t>& body()
	{
		return data_;
	}

	size_t body_length() const
	{
		return body_length_;
	}

	uint32_t type()
	{
		return header_.type;
	}
private:
	RDA_MessageHeader header_;
	std::vector<uint8_t> data_;
	size_t body_length_;
};

// message queue
// just regular queue functions but with some mutual exclusion for
// safety's sake
class TCP_Message_Queue
{
public:

	void push_back(const TCP_Message &data)
	{
		boost::lock_guard<boost::mutex> l(m_);
		q_.push_back(data);
	}

	TCP_Message & front()
	{
		boost::lock_guard<boost::mutex> l(m_);
		TCP_Message &f = g_.front();
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
	std::deque<TCP_Message> q_;
};



// this is the object responisible for reading messages  
// from a server over a TCP socket
// write functionality is missing, because I don't need it yet
class TCP_Client
{
public:

	TCP_Client(boost::asio::io_service& io_service, tcp::socket& socket) :
	  io_service_(io_service), socket_(socket), is_connected_true)
	  {
		  // start reading message header from socket
		  boost::asio::async_read(socket_,
			  boost::asio::buffer(read_msg_.header(), TCP_MEssage::header_length),
			  boost::bind(&TCP_Client::handle_read_header, this, boost::asio::placeholders::error));
		  // start a new io thread
		  io_thread_.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_, ec_)));
	  }

	~TCP_Client()
	{
		if(io_threade_->joinable())io_thread_->join();
	}

	bool is_connected()
	{
		return is_connected_;
	}

	//void parse_msg(std::string msg){
	//	parse_pupil_msg(msg);
	//	parse_event_msg(msg);
	//}

	//double *get_pupil_data(void){
	//	return pupil_data;
	//}

	//std::string get_event_data(int which){
	//	int i = 0;
	//	for(std::vector<std::string>::iterator it = event_data.begin(); i<which; ++it, i++);
	//	return *it;
	//}

private:

	void handle_read_header(const boost::system:;error_code& error)
	{
		if(!error && read_msg_.decode_header())
		{
			// start reading message body
			boost::asio::async_read(socket_, 
				boost::asio::buffer(read_msg_.body()),
				//boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
				boost::bind(&TCP_Client::handle_read_body, this, boost::asio::placeholders::error));
		}
		else do_close();
	}

	void handle_read_body(const boost::system::error_code& error)
	{
		if(!error)
		{
			// put message into queue
			read_msgs_.push_back(read_msg_);
			// start reading the next message header
			boost::asio::async_read(socket_,
				boost::asio::buffer(read_msg_.header(), TCP_message::header_length),
				boost::bind(&TCP_Client::handle_read_header, this,
				boost::asio::placeholders::error));

		}
		else do_close();
	}

	void do_close()
	{
		boost::system::error_code ec;
		socket_.close(ec);
		is_connected_ = false;
	}

private:

	boost::asio::iso_service& io_service_;
	tcp::socket& socket_;
	boost::shared_ptr<boost::thread> io_thread_;
	bool is_connected_;
	boost::system::error_code ec_;

	TCP_Message read_msg_;
	TCP_Message_Queue read_msgs_;
	//void parse_pupil_msg(std::string msg);
	//void parse_event_msg(std::string msg);
	//
	//double pupil_data[7]; // container for pupil data
	//std::vector<std::sring> event_data; // variable length buffer of event markers

};

#endif // PUPILPROCLIENT_H