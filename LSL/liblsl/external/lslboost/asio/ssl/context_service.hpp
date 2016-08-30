//
// ssl/context_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_SSL_CONTEXT_SERVICE_HPP
#define BOOST_ASIO_SSL_CONTEXT_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>
  
#if defined(BOOST_ASIO_ENABLE_OLD_SSL)
# include <lslboost/asio/ssl/old/context_service.hpp>
#endif // defined(BOOST_ASIO_ENABLE_OLD_SSL)

#include <lslboost/asio/detail/push_options.hpp>

namespace lslboost {
namespace asio {
namespace ssl {

#if defined(BOOST_ASIO_ENABLE_OLD_SSL)

using lslboost::asio::ssl::old::context_service;

#endif // defined(BOOST_ASIO_ENABLE_OLD_SSL)

} // namespace ssl
} // namespace asio
} // namespace lslboost

#include <lslboost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_SSL_CONTEXT_SERVICE_HPP
