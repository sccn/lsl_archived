//
// detail/addressof.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_ADDRESSOF_HPP
#define BOOST_ASIO_DETAIL_ADDRESSOF_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_HAS_STD_ADDRESSOF)
# include <memory>
#else // defined(BOOST_ASIO_HAS_STD_ADDRESSOF)
# include <lslboost/utility/addressof.hpp>
#endif // defined(BOOST_ASIO_HAS_STD_ADDRESSOF)

namespace lslboost {
namespace asio {
namespace detail {

#if defined(BOOST_ASIO_HAS_STD_ADDRESSOF)
using std::addressof;
#else // defined(BOOST_ASIO_HAS_STD_ADDRESSOF)
using lslboost::addressof;
#endif // defined(BOOST_ASIO_HAS_STD_ADDRESSOF)

} // namespace detail
} // namespace asio
} // namespace lslboost

#endif // BOOST_ASIO_DETAIL_ADDRESSOF_HPP
