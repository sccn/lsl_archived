#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/uuid/uuid.hpp>
#include <cstdio>
#include <cstring>
#include <vector>
#include <boost/cstdint.hpp>

using boost::asio::ip::tcp;
using namespace boost;

// we create a generic header type, leaving the body type to be customized
//#pragma pack(1)
struct TCP_MessageHeader
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
	
	enum { HEADER_LENGTH = sizeof(TCP_MessageHeader) };
	TCP_Message():body_length_(0){}
	//boost::asio::streambuf response_;

	void decode_header()
	{
		std::cout << "decoding header...\n";
      // Process the response headers.
	
		body_length_ = header_.size - sizeof(TCP_MessageHeader); // this is some huge number for some reason
		//data_.resize(body_length_); // hence this crashes

	}

	void decode_body()
	{

	}

	// utility public member functions
	size_t length() const
	{
		return  sizeof(TCP_MessageHeader) + body_length_;
	}

	size_t header_length() const
	{
		return sizeof(TCP_MessageHeader);
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
	void print_message()
	{
		std::string str(body().begin(), body().end());
		std::cout << str;
	}
private:
	TCP_MessageHeader header_;
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

	TCP_Message& front()
	{
		boost::lock_guard<boost::mutex> l(m_);
		TCP_Message &f = q_.front();		
		f.print_message();
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
	  io_service_(io_service), socket_(socket), is_connected_(true)
	  {
		  // start reading message header from socket
		  boost::asio::async_read(socket_,
			  boost::asio::buffer(read_msg_.header(), TCP_Message::HEADER_LENGTH),
			  boost::bind(&TCP_Client::handle_read_header, this, boost::asio::placeholders::error));
		  // start a new io thread
		  io_thread_.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_, ec_)));
	  }

	~TCP_Client()
	{
		if(io_thread_->joinable())io_thread_->join();
	}

	bool is_connected()
	{
		return is_connected_;
	}

	TCP_Message_Queue& messages(void)
	{
		return read_msgs_;
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

	void handle_read_header(const boost::system::error_code& error)
	{
		if(!error)// && read_msg_.decode_header())
		{
			std::cout << "reading header...\n";
			read_msg_.decode_header();
			// start reading message body
			boost::asio::async_read(socket_, 
				//boost::asio::buffer(read_msg_.body()),
				boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
				boost::bind(&TCP_Client::handle_read_body, this, boost::asio::placeholders::error));
		}
		else do_close();
	}

	void handle_read_body(const boost::system::error_code& error)
	{
		if(!error)
		{
			// put message into queue
			//read_msg_.print_message();
			std::cout << "reading body...\n";
			read_msgs_.push_back(read_msg_);
			// start reading the next message header
			boost::asio::async_read(socket_,
				boost::asio::buffer(read_msg_.header(), TCP_Message::HEADER_LENGTH),
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

	boost::asio::io_service& io_service_;
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

#endif // TCPCLIENT_H