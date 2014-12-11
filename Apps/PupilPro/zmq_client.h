#include <zmq.hpp>

class ZMQ_Client
{

public:
	ZMQ_Client(std::string ip) :
	  context_(1), socket_(context_, ZMQ_SUB), message_(MAX_MSG_LEN)
	  {
		  socket_.setsockopt(ZMQ_IDENTITY,"Hello",5);
		  socket_.setsockopt(ZMQ_SUBSCRIBE,"",0);
		  socket_.connect(ip.c_str());
	  }

	  void socket_pull(void)
	  {
		  socket_.recv(&message_);
	  }

	  std::string msg_string(void)
	  {
		  std::string str = std::string(static_cast<char *>(message_.data()), message_.size());
		  return str;
	  }

private:

	zmq::message_t message_;
	zmq::context_t context_;
	zmq::socket_t socket_;
	static const int MAX_MSG_LEN=4096;
};