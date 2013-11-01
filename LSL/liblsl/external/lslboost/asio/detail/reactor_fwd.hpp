//
// detail/reactor_fwd.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_REACTOR_FWD_HPP
#define BOOST_ASIO_DETAIL_REACTOR_FWD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_HAS_IOCP)
# include <lslboost/asio/detail/select_reactor_fwd.hpp>
#elif defined(BOOST_ASIO_HAS_EPOLL)
# include <lslboost/asio/detail/epoll_reactor_fwd.hpp>
#elif defined(BOOST_ASIO_HAS_KQUEUE)
# include <lslboost/asio/detail/kqueue_reactor_fwd.hpp>
#elif defined(BOOST_ASIO_HAS_DEV_POLL)
# include <lslboost/asio/detail/dev_poll_reactor_fwd.hpp>
#else
# include <lslboost/asio/detail/select_reactor_fwd.hpp>
#endif

namespace lslboost {
namespace asio {
namespace detail {

#if defined(BOOST_ASIO_HAS_IOCP)
typedef select_reactor reactor;
#elif defined(BOOST_ASIO_HAS_EPOLL)
typedef epoll_reactor reactor;
#elif defined(BOOST_ASIO_HAS_KQUEUE)
typedef kqueue_reactor reactor;
#elif defined(BOOST_ASIO_HAS_DEV_POLL)
typedef dev_poll_reactor reactor;
#else
typedef select_reactor reactor;
#endif

} // namespace detail
} // namespace asio
} // namespace lslboost

#endif // BOOST_ASIO_DETAIL_REACTOR_FWD_HPP
