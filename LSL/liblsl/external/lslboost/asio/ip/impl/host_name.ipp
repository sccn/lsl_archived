//
// ip/impl/host_name.ipp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_IP_IMPL_HOST_NAME_IPP
#define BOOST_ASIO_IP_IMPL_HOST_NAME_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>
#include <lslboost/asio/detail/socket_ops.hpp>
#include <lslboost/asio/detail/throw_error.hpp>
#include <lslboost/asio/detail/winsock_init.hpp>
#include <lslboost/asio/ip/host_name.hpp>

#include <lslboost/asio/detail/push_options.hpp>

namespace lslboost {
namespace asio {
namespace ip {

std::string host_name()
{
  char name[1024];
  lslboost::system::error_code ec;
  if (lslboost::asio::detail::socket_ops::gethostname(name, sizeof(name), ec) != 0)
  {
    lslboost::asio::detail::throw_error(ec);
    return std::string();
  }
  return std::string(name);
}

std::string host_name(lslboost::system::error_code& ec)
{
  char name[1024];
  if (lslboost::asio::detail::socket_ops::gethostname(name, sizeof(name), ec) != 0)
    return std::string();
  return std::string(name);
}

} // namespace ip
} // namespace asio
} // namespace lslboost

#include <lslboost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_IP_IMPL_HOST_NAME_IPP
