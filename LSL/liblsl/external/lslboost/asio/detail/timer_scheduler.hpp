//
// detail/timer_scheduler.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_TIMER_SCHEDULER_HPP
#define BOOST_ASIO_DETAIL_TIMER_SCHEDULER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>
#include <lslboost/asio/detail/timer_scheduler_fwd.hpp>

#if defined(BOOST_ASIO_WINDOWS_RUNTIME)
# include <lslboost/asio/detail/winrt_timer_scheduler.hpp>
#elif defined(BOOST_ASIO_HAS_IOCP)
# include <lslboost/asio/detail/win_iocp_io_service.hpp>
#elif defined(BOOST_ASIO_HAS_EPOLL)
# include <lslboost/asio/detail/epoll_reactor.hpp>
#elif defined(BOOST_ASIO_HAS_KQUEUE)
# include <lslboost/asio/detail/kqueue_reactor.hpp>
#elif defined(BOOST_ASIO_HAS_DEV_POLL)
# include <lslboost/asio/detail/dev_poll_reactor.hpp>
#else
# include <lslboost/asio/detail/select_reactor.hpp>
#endif

#endif // BOOST_ASIO_DETAIL_TIMER_SCHEDULER_HPP
