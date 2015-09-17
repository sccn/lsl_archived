//
// impl/src.hpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_IMPL_SRC_HPP
#define BOOST_ASIO_IMPL_SRC_HPP

#define BOOST_ASIO_SOURCE

#include <lslboost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_HEADER_ONLY)
# error Do not compile Asio library source with BOOST_ASIO_HEADER_ONLY defined
#endif

#include <lslboost/asio/impl/error.ipp>
#include <lslboost/asio/impl/handler_alloc_hook.ipp>
#include <lslboost/asio/impl/io_service.ipp>
#include <lslboost/asio/impl/serial_port_base.ipp>
#include <lslboost/asio/detail/impl/buffer_sequence_adapter.ipp>
#include <lslboost/asio/detail/impl/descriptor_ops.ipp>
#include <lslboost/asio/detail/impl/dev_poll_reactor.ipp>
#include <lslboost/asio/detail/impl/epoll_reactor.ipp>
#include <lslboost/asio/detail/impl/eventfd_select_interrupter.ipp>
#include <lslboost/asio/detail/impl/handler_tracking.ipp>
#include <lslboost/asio/detail/impl/kqueue_reactor.ipp>
#include <lslboost/asio/detail/impl/pipe_select_interrupter.ipp>
#include <lslboost/asio/detail/impl/posix_event.ipp>
#include <lslboost/asio/detail/impl/posix_mutex.ipp>
#include <lslboost/asio/detail/impl/posix_thread.ipp>
#include <lslboost/asio/detail/impl/posix_tss_ptr.ipp>
#include <lslboost/asio/detail/impl/reactive_descriptor_service.ipp>
#include <lslboost/asio/detail/impl/reactive_serial_port_service.ipp>
#include <lslboost/asio/detail/impl/reactive_socket_service_base.ipp>
#include <lslboost/asio/detail/impl/resolver_service_base.ipp>
#include <lslboost/asio/detail/impl/select_reactor.ipp>
#include <lslboost/asio/detail/impl/service_registry.ipp>
#include <lslboost/asio/detail/impl/signal_set_service.ipp>
#include <lslboost/asio/detail/impl/socket_ops.ipp>
#include <lslboost/asio/detail/impl/socket_select_interrupter.ipp>
#include <lslboost/asio/detail/impl/strand_service.ipp>
#include <lslboost/asio/detail/impl/task_io_service.ipp>
#include <lslboost/asio/detail/impl/throw_error.ipp>
#include <lslboost/asio/detail/impl/timer_queue_ptime.ipp>
#include <lslboost/asio/detail/impl/timer_queue_set.ipp>
#include <lslboost/asio/detail/impl/win_iocp_handle_service.ipp>
#include <lslboost/asio/detail/impl/win_iocp_io_service.ipp>
#include <lslboost/asio/detail/impl/win_iocp_serial_port_service.ipp>
#include <lslboost/asio/detail/impl/win_iocp_socket_service_base.ipp>
#include <lslboost/asio/detail/impl/win_event.ipp>
#include <lslboost/asio/detail/impl/win_mutex.ipp>
#include <lslboost/asio/detail/impl/win_object_handle_service.ipp>
#include <lslboost/asio/detail/impl/win_static_mutex.ipp>
#include <lslboost/asio/detail/impl/win_thread.ipp>
#include <lslboost/asio/detail/impl/win_tss_ptr.ipp>
#include <lslboost/asio/detail/impl/winrt_ssocket_service_base.ipp>
#include <lslboost/asio/detail/impl/winrt_timer_scheduler.ipp>
#include <lslboost/asio/detail/impl/winsock_init.ipp>
#include <lslboost/asio/generic/detail/impl/endpoint.ipp>
#include <lslboost/asio/ip/impl/address.ipp>
#include <lslboost/asio/ip/impl/address_v4.ipp>
#include <lslboost/asio/ip/impl/address_v6.ipp>
#include <lslboost/asio/ip/impl/host_name.ipp>
#include <lslboost/asio/ip/detail/impl/endpoint.ipp>
#include <lslboost/asio/local/detail/impl/endpoint.ipp>

#endif // BOOST_ASIO_IMPL_SRC_HPP
