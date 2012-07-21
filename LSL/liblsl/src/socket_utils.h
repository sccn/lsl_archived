#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include "version.h"
#include "api_config.h"
#include <boost/asio.hpp>

namespace lsl {
    
	/**
     * Bind a socket (or acceptor) to a free port in the configured port range or throw an error otherwise.
     */
	template<class Socket, class Protocol> int bind_port_in_range(Socket &sock, Protocol protocol) {
		for (int k=0,e=api_config::get_instance()->port_range(); k<e; k++) {
			try {
				sock.bind(typename Protocol::endpoint(protocol,(unsigned short)(k + api_config::get_instance()->base_port())));
				return k + api_config::get_instance()->base_port();
			} catch (boost::system::system_error &) { /* port occupied */ }
		}
		throw std::runtime_error("All local ports were found occupied. You may have more open outlets on this machine than your PortRange setting allows (see Configuration File in the lab streaming layer API docs) or you have a problem with your network configuration.");
	}
}

#endif

