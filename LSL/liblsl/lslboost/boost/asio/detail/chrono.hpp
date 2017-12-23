//
// detail/chrono.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_CHRONO_HPP
#define BOOST_ASIO_DETAIL_CHRONO_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_HAS_STD_CHRONO)
# include <chrono>
#elif defined(BOOST_ASIO_HAS_BOOST_CHRONO)
# include <boost/chrono/system_clocks.hpp>
#endif // defined(BOOST_ASIO_HAS_BOOST_CHRONO)

namespace lslboost {
namespace asio {
namespace chrono {

#if defined(BOOST_ASIO_HAS_STD_CHRONO)
using std::chrono::duration;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::minutes;
using std::chrono::hours;
using std::chrono::time_point_cast;
#if defined(BOOST_ASIO_HAS_STD_CHRONO_MONOTONIC_CLOCK)
typedef std::chrono::monotonic_clock steady_clock;
#else // defined(BOOST_ASIO_HAS_STD_CHRONO_MONOTONIC_CLOCK)
using std::chrono::steady_clock;
#endif // defined(BOOST_ASIO_HAS_STD_CHRONO_MONOTONIC_CLOCK)
using std::chrono::system_clock;
using std::chrono::high_resolution_clock;
#elif defined(BOOST_ASIO_HAS_BOOST_CHRONO)
using lslboost::chrono::duration;
using lslboost::chrono::time_point;
using lslboost::chrono::duration_cast;
using lslboost::chrono::nanoseconds;
using lslboost::chrono::microseconds;
using lslboost::chrono::milliseconds;
using lslboost::chrono::seconds;
using lslboost::chrono::minutes;
using lslboost::chrono::hours;
using lslboost::chrono::time_point_cast;
using lslboost::chrono::system_clock;
using lslboost::chrono::steady_clock;
using lslboost::chrono::high_resolution_clock;
#endif // defined(BOOST_ASIO_HAS_BOOST_CHRONO)

} // namespace chrono
} // namespace asio
} // namespace lslboost

#endif // BOOST_ASIO_DETAIL_CHRONO_HPP
