/*
 * Copyright (c) 2013-present, The Eye Tribe. 
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in
 * the LICENSE file in the root directory of this source tree. 
 *
 */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX
#endif

#include "gazeapi_socket.hpp"


namespace gtl
{
    JSONPackageMatcher::JSONPackageMatcher()
        : m_stack(0)
        , m_in_message(false)
    {}

    Socket::Socket(bool verbose)
        : m_io_service()
        , m_socket(m_io_service)
        , m_verbose(verbose)
    {}

    Socket::~Socket()
    {
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    bool Socket::connect(std::string const & address, std::string const & port)
    {
        // If a thread is already running - wait for it to finish
        if (m_thread.joinable())
        {
            m_thread.join();
        }

        using namespace boost::asio::ip;

        tcp::resolver resolver(m_io_service);
        tcp::resolver::query query(address, port);
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;
        boost::system::error_code error = boost::asio::error::host_not_found;

        while (error && endpoint_iterator != end)
        {
            m_socket.close();
            m_socket.connect(*endpoint_iterator++, error);
        }

        if (error)
        {
            m_socket.close();
            return false;
        }

        boost::asio::async_read_until(m_socket, m_buffer, m_matcher,
            boost::bind(&Socket::on_read,
            this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

        // Keep io_service processing requests in a separate thread
        m_thread = boost::thread(boost::bind((size_t (boost::asio::io_service::*)()) &boost::asio::io_service::run, &m_io_service));

        return true;
    }

    void Socket::disconnect()
    {
        if (m_socket.is_open())
        {
            //m_socket.shutdown(boost::asio::socket_base::shutdown_type::shutdown_both);
            m_socket.close();
        }

        m_io_service.stop(); // stops io_service and exits thread
    }

    bool Socket::send(std::string const & message)
    {
        size_t const size = message.size();
        char* data = new char[size];
        memcpy(data, message.c_str(), size);

        boost::asio::async_write(m_socket,
            boost::asio::buffer(data, size),
            boost::bind(&Socket::on_write, this, boost::asio::placeholders::error, data));

        if (m_verbose)
        {
            std::cout << "Send: " << message << std::endl << std::flush;
        }

        return true;
    }

    void Socket::on_read(const boost::system::error_code& error, size_t bytes_transferred)
    {
        Observable<ISocketListener>::ObserverVector const & observers = get_observers();

        if (error)
        {
            for (size_t i = 0; i < observers.size(); ++i)
            {
                observers[i]->on_disconnected();
            }
        }
        else
        {
            typedef boost::asio::streambuf::const_buffers_type buffer_type;
            typedef boost::asio::buffers_iterator<buffer_type> iterator;

            buffer_type input = m_buffer.data();

            std::string reply; reply.reserve(bytes_transferred);
            std::copy(iterator::begin(input), iterator::begin(input)+bytes_transferred, std::back_inserter(reply));
            m_buffer.consume(bytes_transferred);

            for (size_t i = 0; i < observers.size(); ++i)
            {
                observers[i]->on_message(reply);
            }

            boost::asio::async_read_until(m_socket, m_buffer, m_matcher,
                boost::bind(&Socket::on_read,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));

            if (m_verbose)
            {
                std::cout << "Recv: " << reply << std::endl << std::flush;
            }
        }
    }

    void Socket::on_write(const boost::system::error_code& error, char* data)
    {
        if (error)
        {
            Observable<ISocketListener>::ObserverVector const & observers = get_observers();

            for (size_t i = 0; i < observers.size(); ++i)
            {
                observers[i]->on_disconnected();
            }
        }

        delete[] data;
    }
}
