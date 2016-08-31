//
// asio.hpp
// ~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
//  See www.lslboost.org/libs/asio for documentation.
//

#ifndef BOOST_ASIO_HPP
#define BOOST_ASIO_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/async_result.hpp>
#include <lslboost/asio/basic_datagram_socket.hpp>
#include <lslboost/asio/basic_deadline_timer.hpp>
#include <lslboost/asio/basic_io_object.hpp>
#include <lslboost/asio/basic_raw_socket.hpp>
#include <lslboost/asio/basic_seq_packet_socket.hpp>
#include <lslboost/asio/basic_serial_port.hpp>
#include <lslboost/asio/basic_signal_set.hpp>
#include <lslboost/asio/basic_socket_acceptor.hpp>
#include <lslboost/asio/basic_socket_iostream.hpp>
#include <lslboost/asio/basic_socket_streambuf.hpp>
#include <lslboost/asio/basic_stream_socket.hpp>
#include <lslboost/asio/basic_streambuf.hpp>
#include <lslboost/asio/basic_waitable_timer.hpp>
#include <lslboost/asio/buffer.hpp>
#include <lslboost/asio/buffered_read_stream_fwd.hpp>
#include <lslboost/asio/buffered_read_stream.hpp>
#include <lslboost/asio/buffered_stream_fwd.hpp>
#include <lslboost/asio/buffered_stream.hpp>
#include <lslboost/asio/buffered_write_stream_fwd.hpp>
#include <lslboost/asio/buffered_write_stream.hpp>
#include <lslboost/asio/buffers_iterator.hpp>
#include <lslboost/asio/completion_condition.hpp>
#include <lslboost/asio/connect.hpp>
#include <lslboost/asio/coroutine.hpp>
#include <lslboost/asio/datagram_socket_service.hpp>
#include <lslboost/asio/deadline_timer_service.hpp>
#include <lslboost/asio/deadline_timer.hpp>
#include <lslboost/asio/error.hpp>
#include <lslboost/asio/generic/basic_endpoint.hpp>
#include <lslboost/asio/generic/datagram_protocol.hpp>
#include <lslboost/asio/generic/raw_protocol.hpp>
#include <lslboost/asio/generic/seq_packet_protocol.hpp>
#include <lslboost/asio/generic/stream_protocol.hpp>
#include <lslboost/asio/handler_alloc_hook.hpp>
#include <lslboost/asio/handler_continuation_hook.hpp>
#include <lslboost/asio/handler_invoke_hook.hpp>
#include <lslboost/asio/handler_type.hpp>
#include <lslboost/asio/io_service.hpp>
#include <lslboost/asio/ip/address.hpp>
#include <lslboost/asio/ip/address_v4.hpp>
#include <lslboost/asio/ip/address_v6.hpp>
#include <lslboost/asio/ip/basic_endpoint.hpp>
#include <lslboost/asio/ip/basic_resolver.hpp>
#include <lslboost/asio/ip/basic_resolver_entry.hpp>
#include <lslboost/asio/ip/basic_resolver_iterator.hpp>
#include <lslboost/asio/ip/basic_resolver_query.hpp>
#include <lslboost/asio/ip/host_name.hpp>
#include <lslboost/asio/ip/icmp.hpp>
#include <lslboost/asio/ip/multicast.hpp>
#include <lslboost/asio/ip/resolver_query_base.hpp>
#include <lslboost/asio/ip/resolver_service.hpp>
#include <lslboost/asio/ip/tcp.hpp>
#include <lslboost/asio/ip/udp.hpp>
#include <lslboost/asio/ip/unicast.hpp>
#include <lslboost/asio/ip/v6_only.hpp>
#include <lslboost/asio/is_read_buffered.hpp>
#include <lslboost/asio/is_write_buffered.hpp>
#include <lslboost/asio/local/basic_endpoint.hpp>
#include <lslboost/asio/local/connect_pair.hpp>
#include <lslboost/asio/local/datagram_protocol.hpp>
#include <lslboost/asio/local/stream_protocol.hpp>
#include <lslboost/asio/placeholders.hpp>
#include <lslboost/asio/posix/basic_descriptor.hpp>
#include <lslboost/asio/posix/basic_stream_descriptor.hpp>
#include <lslboost/asio/posix/descriptor_base.hpp>
#include <lslboost/asio/posix/stream_descriptor.hpp>
#include <lslboost/asio/posix/stream_descriptor_service.hpp>
#include <lslboost/asio/raw_socket_service.hpp>
#include <lslboost/asio/read.hpp>
#include <lslboost/asio/read_at.hpp>
#include <lslboost/asio/read_until.hpp>
#include <lslboost/asio/seq_packet_socket_service.hpp>
#include <lslboost/asio/serial_port.hpp>
#include <lslboost/asio/serial_port_base.hpp>
#include <lslboost/asio/serial_port_service.hpp>
#include <lslboost/asio/signal_set.hpp>
#include <lslboost/asio/signal_set_service.hpp>
#include <lslboost/asio/socket_acceptor_service.hpp>
#include <lslboost/asio/socket_base.hpp>
#include <lslboost/asio/strand.hpp>
#include <lslboost/asio/stream_socket_service.hpp>
#include <lslboost/asio/streambuf.hpp>
#include <lslboost/asio/time_traits.hpp>
#include <lslboost/asio/version.hpp>
#include <lslboost/asio/wait_traits.hpp>
#include <lslboost/asio/waitable_timer_service.hpp>
#include <lslboost/asio/windows/basic_handle.hpp>
#include <lslboost/asio/windows/basic_object_handle.hpp>
#include <lslboost/asio/windows/basic_random_access_handle.hpp>
#include <lslboost/asio/windows/basic_stream_handle.hpp>
#include <lslboost/asio/windows/object_handle.hpp>
#include <lslboost/asio/windows/object_handle_service.hpp>
#include <lslboost/asio/windows/overlapped_ptr.hpp>
#include <lslboost/asio/windows/random_access_handle.hpp>
#include <lslboost/asio/windows/random_access_handle_service.hpp>
#include <lslboost/asio/windows/stream_handle.hpp>
#include <lslboost/asio/windows/stream_handle_service.hpp>
#include <lslboost/asio/write.hpp>
#include <lslboost/asio/write_at.hpp>

#endif // BOOST_ASIO_HPP
