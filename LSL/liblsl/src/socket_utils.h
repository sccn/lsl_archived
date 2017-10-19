#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include "common.h"
#include "api_config.h"
#include <boost/cstdint.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>

namespace lsl {
    
    /// Bind a socket (or acceptor) to a free port in the configured port range or throw an error otherwise.
	template<class Socket, class Protocol> int bind_port_in_range(Socket &sock, Protocol protocol) {
		for (int k=0,e=api_config::get_instance()->port_range(); k<e; k++) {
			try {
				sock.bind(typename Protocol::endpoint(protocol,(unsigned short)(k + api_config::get_instance()->base_port())));
				return k + api_config::get_instance()->base_port();
			} catch (boost::system::system_error &) { /* port occupied */ }
		}
		if (api_config::get_instance()->allow_random_ports()) {
			while (true) {
				unsigned short port = 1025 + rand()%64000;
				try {
					sock.bind(typename Protocol::endpoint(protocol,port));
					return port;
				} catch (boost::system::system_error &) { /* port occupied */ }
			}
		} else
			throw std::runtime_error("All local ports were found occupied. You may have more open outlets on this machine than your PortRange setting allows (see Network Connectivity in the LSL wiki) or you have a problem with your network configuration.");
	}
    
    /// Bind to and listen at a socket (or acceptor) on a free port in the configured port range or throw an error otherwise.
	template<class Socket, class Protocol> int bind_and_listen_to_port_in_range(Socket &sock, Protocol protocol, int backlog) {
		for (int k=0,e=api_config::get_instance()->port_range(); k<e; k++) {
			try {
				sock.bind(typename Protocol::endpoint(protocol,(unsigned short)(k + api_config::get_instance()->base_port())));
                sock.listen(backlog);
				return k + api_config::get_instance()->base_port();
			} catch (boost::system::system_error &) { /* port occupied */ }
		}
		if (api_config::get_instance()->allow_random_ports()) {
			while (true) {
				unsigned short port = 1025 + rand()%64000;
				try {
					sock.bind(typename Protocol::endpoint(protocol,port));
					sock.listen(backlog);
					return port;
				} catch (boost::system::system_error &) { /* port occupied */ }
			}
		} else
			throw std::runtime_error("All local ports were found occupied. You may have more open outlets on this machine than your PortRange setting allows (see Network Connectivity in the LSL wiki) or you have a problem with your network configuration.");
	}
    
	/// Gracefully close a socket.
	template<class SocketPtr> void close_if_open(SocketPtr sock) {
		try {
			if (sock->is_open())
				sock->close();
		}  catch(std::exception &e) {
			std::cerr << "Error during close_if_open (thread id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
		}
	}

	/// Gracefully shut down a socket.
	template<class SocketPtr, class Protocol> void shutdown_and_close(SocketPtr sock) {
		try {
			if (sock->is_open()) {
				try {
					// (in some cases shutdown may fail)
					sock->shutdown(Protocol::socket::shutdown_both);
				} catch(...) {}
				sock->close();
			}
		}  catch(std::exception &e) {
			std::cerr << "Error during shutdown_and_close (thread id: " << boost::this_thread::get_id() << "): " << e.what() << std::endl;
		}
	}

	/// Measure the endian conversion performance of this machine.
	double measure_endian_performance();
}

#endif

