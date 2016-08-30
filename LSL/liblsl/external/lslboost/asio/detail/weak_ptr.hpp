//
// detail/weak_ptr.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_WEAK_PTR_HPP
#define BOOST_ASIO_DETAIL_WEAK_PTR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_HAS_STD_SHARED_PTR)
# include <memory>
#else // defined(BOOST_ASIO_HAS_STD_SHARED_PTR)
# include <lslboost/weak_ptr.hpp>
#endif // defined(BOOST_ASIO_HAS_STD_SHARED_PTR)

namespace lslboost {
namespace asio {
namespace detail {

#if defined(BOOST_ASIO_HAS_STD_SHARED_PTR)
using std::weak_ptr;
#else // defined(BOOST_ASIO_HAS_STD_SHARED_PTR)
using lslboost::weak_ptr;
#endif // defined(BOOST_ASIO_HAS_STD_SHARED_PTR)

} // namespace detail
} // namespace asio
} // namespace lslboost

#endif // BOOST_ASIO_DETAIL_WEAK_PTR_HPP
