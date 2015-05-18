/*
 * Copyright (c) 2013-present, The Eye Tribe. 
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in
 * the LICENSE file in the root directory of this source tree. 
 *
 */

#ifndef _THEEYETRIBE_GAZEAPI_SOCKET_H_
#define _THEEYETRIBE_GAZEAPI_SOCKET_H_

#include "gazeapi_observable.hpp"

#include <boost/asio.hpp>
#include <boost/timer.hpp>
#include <boost/thread.hpp>


namespace gtl
{
    class JSONPackageMatcher; // Forward declaration for boost::asio::is_matching_condition
};

namespace boost
{
    namespace asio
    {
        template <> struct is_match_condition<gtl::JSONPackageMatcher>
        : public true_type
        {};
    }
};

namespace gtl
{
    class JSONPackageMatcher
    {
    public:
        JSONPackageMatcher();

        template <typename Iterator>
        std::pair<Iterator, bool> operator()(Iterator begin, Iterator end)
        {
            Iterator i = begin;

            while (i != end)
            {
                bool const left = *i == '{';
                bool const right = *i == '}';
                m_stack += left ? 1 : right ? -1 : 0;
                m_in_message |= left;

                ++i; // We need to make sure that '}' is consumed before returning

                if (m_stack == 0 && m_in_message)
                {
                    while (i != end && *i != '{')
                    {
                        ++i; // read all post-amble until next message starts (\r, \n, etc.)
                    }

                    m_in_message = false;
                    return std::make_pair(i, true);
                }
            }
            return std::make_pair(i, false);
        }

    private:
        bool    m_in_message;
        size_t  m_stack;
    };

    // Call backs from socket
    class ISocketListener
    {
    public:
        virtual ~ISocketListener() {}
        virtual void on_message(std::string const & message) = 0;
        virtual void on_disconnected() = 0;
    };

    class Socket : public Observable<ISocketListener>
    {
    public:
        Socket(bool verbose = false);
        ~Socket();

        bool connect(std::string const & address, std::string const & port);
        void disconnect();
        bool send(std::string const & message);

    private:
        void on_read(const boost::system::error_code& error, size_t bytes_transferred);
        void on_write(const boost::system::error_code& error, char* data);

    private:
        JSONPackageMatcher              m_matcher;
        boost::asio::streambuf          m_buffer;
        boost::thread                   m_thread;
        boost::asio::io_service         m_io_service;
        boost::asio::ip::tcp::socket    m_socket;
        bool                            m_verbose;
    };
}

#endif // _THEEYETRIBE_GAZEAPI_SOCKET_H_
