//
// impl/io_service.ipp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_IMPL_IO_SERVICE_IPP
#define BOOST_ASIO_IMPL_IO_SERVICE_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>
#include <lslboost/asio/io_service.hpp>
#include <lslboost/asio/detail/limits.hpp>
#include <lslboost/asio/detail/scoped_ptr.hpp>
#include <lslboost/asio/detail/service_registry.hpp>
#include <lslboost/asio/detail/throw_error.hpp>

#if defined(BOOST_ASIO_HAS_IOCP)
# include <lslboost/asio/detail/win_iocp_io_service.hpp>
#else
# include <lslboost/asio/detail/task_io_service.hpp>
#endif

#include <lslboost/asio/detail/push_options.hpp>

namespace lslboost {
namespace asio {

io_service::io_service()
  : service_registry_(new lslboost::asio::detail::service_registry(
        *this, static_cast<impl_type*>(0),
        (std::numeric_limits<std::size_t>::max)())),
    impl_(service_registry_->first_service<impl_type>())
{
}

io_service::io_service(std::size_t concurrency_hint)
  : service_registry_(new lslboost::asio::detail::service_registry(
        *this, static_cast<impl_type*>(0), concurrency_hint)),
    impl_(service_registry_->first_service<impl_type>())
{
}

io_service::~io_service()
{
  delete service_registry_;
}

std::size_t io_service::run()
{
  lslboost::system::error_code ec;
  std::size_t s = impl_.run(ec);
  lslboost::asio::detail::throw_error(ec);
  return s;
}

std::size_t io_service::run(lslboost::system::error_code& ec)
{
  return impl_.run(ec);
}

std::size_t io_service::run_one()
{
  lslboost::system::error_code ec;
  std::size_t s = impl_.run_one(ec);
  lslboost::asio::detail::throw_error(ec);
  return s;
}

std::size_t io_service::run_one(lslboost::system::error_code& ec)
{
  return impl_.run_one(ec);
}

std::size_t io_service::poll()
{
  lslboost::system::error_code ec;
  std::size_t s = impl_.poll(ec);
  lslboost::asio::detail::throw_error(ec);
  return s;
}

std::size_t io_service::poll(lslboost::system::error_code& ec)
{
  return impl_.poll(ec);
}

std::size_t io_service::poll_one()
{
  lslboost::system::error_code ec;
  std::size_t s = impl_.poll_one(ec);
  lslboost::asio::detail::throw_error(ec);
  return s;
}

std::size_t io_service::poll_one(lslboost::system::error_code& ec)
{
  return impl_.poll_one(ec);
}

void io_service::stop()
{
  impl_.stop();
}

bool io_service::stopped() const
{
  return impl_.stopped();
}

void io_service::reset()
{
  impl_.reset();
}

void io_service::notify_fork(lslboost::asio::io_service::fork_event event)
{
  service_registry_->notify_fork(event);
}

io_service::service::service(lslboost::asio::io_service& owner)
  : owner_(owner),
    next_(0)
{
}

io_service::service::~service()
{
}

void io_service::service::fork_service(lslboost::asio::io_service::fork_event)
{
}

service_already_exists::service_already_exists()
  : std::logic_error("Service already exists.")
{
}

invalid_service_owner::invalid_service_owner()
  : std::logic_error("Invalid service owner.")
{
}

} // namespace asio
} // namespace lslboost

#include <lslboost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_IMPL_IO_SERVICE_IPP
