//
// ssl/error.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_SSL_ERROR_HPP
#define BOOST_ASIO_SSL_ERROR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>
#include <lslboost/system/error_code.hpp>

#include <lslboost/asio/detail/push_options.hpp>

namespace lslboost {
namespace asio {
namespace error {

enum ssl_errors
{
};

extern BOOST_ASIO_DECL
const lslboost::system::error_category& get_ssl_category();

static const lslboost::system::error_category& ssl_category
  = lslboost::asio::error::get_ssl_category();

} // namespace error
} // namespace asio
} // namespace lslboost

namespace lslboost {
namespace system {

template<> struct is_error_code_enum<lslboost::asio::error::ssl_errors>
{
  static const bool value = true;
};

} // namespace system
} // namespace lslboost

namespace lslboost {
namespace asio {
namespace error {

inline lslboost::system::error_code make_error_code(ssl_errors e)
{
  return lslboost::system::error_code(
      static_cast<int>(e), get_ssl_category());
}

} // namespace error
} // namespace asio
} // namespace lslboost

#include <lslboost/asio/detail/pop_options.hpp>

#if defined(BOOST_ASIO_HEADER_ONLY)
# include <lslboost/asio/ssl/impl/error.ipp>
#endif // defined(BOOST_ASIO_HEADER_ONLY)

#endif // BOOST_ASIO_SSL_ERROR_HPP
