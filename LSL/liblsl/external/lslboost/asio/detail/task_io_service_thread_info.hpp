//
// detail/task_io_service_thread_info.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_TASK_IO_SERVICE_THREAD_INFO_HPP
#define BOOST_ASIO_DETAIL_TASK_IO_SERVICE_THREAD_INFO_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/event.hpp>
#include <lslboost/asio/detail/op_queue.hpp>
#include <lslboost/asio/detail/thread_info_base.hpp>

#include <lslboost/asio/detail/push_options.hpp>

namespace lslboost {
namespace asio {
namespace detail {

class task_io_service;
class task_io_service_operation;

struct task_io_service_thread_info : public thread_info_base
{
  event* wakeup_event;
  op_queue<task_io_service_operation> private_op_queue;
  long private_outstanding_work;
  task_io_service_thread_info* next;
};

} // namespace detail
} // namespace asio
} // namespace lslboost

#include <lslboost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_DETAIL_TASK_IO_SERVICE_THREAD_INFO_HPP
