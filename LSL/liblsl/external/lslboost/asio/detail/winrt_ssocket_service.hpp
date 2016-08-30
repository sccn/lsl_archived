//
// detail/winrt_ssocket_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_WINRT_SSOCKET_SERVICE_HPP
#define BOOST_ASIO_DETAIL_WINRT_SSOCKET_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_WINDOWS_RUNTIME)

#include <lslboost/asio/error.hpp>
#include <lslboost/asio/io_service.hpp>
#include <lslboost/asio/detail/addressof.hpp>
#include <lslboost/asio/detail/winrt_socket_connect_op.hpp>
#include <lslboost/asio/detail/winrt_ssocket_service_base.hpp>
#include <lslboost/asio/detail/winrt_utils.hpp>

#include <lslboost/asio/detail/push_options.hpp>

namespace lslboost {
namespace asio {
namespace detail {

template <typename Protocol>
class winrt_ssocket_service :
  public winrt_ssocket_service_base
{
public:
  // The protocol type.
  typedef Protocol protocol_type;

  // The endpoint type.
  typedef typename Protocol::endpoint endpoint_type;

  // The native type of a socket.
  typedef Windows::Networking::Sockets::StreamSocket^ native_handle_type;

  // The implementation type of the socket.
  struct implementation_type : base_implementation_type
  {
    // Default constructor.
    implementation_type()
      : base_implementation_type(),
        protocol_(endpoint_type().protocol())
    {
    }

    // The protocol associated with the socket.
    protocol_type protocol_;
  };

  // Constructor.
  winrt_ssocket_service(lslboost::asio::io_service& io_service)
    : winrt_ssocket_service_base(io_service)
  {
  }

  // Move-construct a new socket implementation.
  void move_construct(implementation_type& impl,
      implementation_type& other_impl)
  {
    this->base_move_construct(impl, other_impl);

    impl.protocol_ = other_impl.protocol_;
    other_impl.protocol_ = endpoint_type().protocol();
  }

  // Move-assign from another socket implementation.
  void move_assign(implementation_type& impl,
      winrt_ssocket_service& other_service,
      implementation_type& other_impl)
  {
    this->base_move_assign(impl, other_service, other_impl);

    impl.protocol_ = other_impl.protocol_;
    other_impl.protocol_ = endpoint_type().protocol();
  }

  // Move-construct a new socket implementation from another protocol type.
  template <typename Protocol1>
  void converting_move_construct(implementation_type& impl,
      typename winrt_ssocket_service<
        Protocol1>::implementation_type& other_impl)
  {
    this->base_move_construct(impl, other_impl);

    impl.protocol_ = protocol_type(other_impl.protocol_);
    other_impl.protocol_ = typename Protocol1::endpoint().protocol();
  }

  // Open a new socket implementation.
  lslboost::system::error_code open(implementation_type& impl,
      const protocol_type& protocol, lslboost::system::error_code& ec)
  {
    if (is_open(impl))
    {
      ec = lslboost::asio::error::already_open;
      return ec;
    }

    try
    {
      impl.socket_ = ref new Windows::Networking::Sockets::StreamSocket;
      impl.protocol_ = protocol;
      ec = lslboost::system::error_code();
    }
    catch (Platform::Exception^ e)
    {
      ec = lslboost::system::error_code(e->HResult,
            lslboost::system::system_category());
    }

    return ec;
  }

  // Assign a native socket to a socket implementation.
  lslboost::system::error_code assign(implementation_type& impl,
      const protocol_type& protocol, const native_handle_type& native_socket,
      lslboost::system::error_code& ec)
  {
    if (is_open(impl))
    {
      ec = lslboost::asio::error::already_open;
      return ec;
    }

    impl.socket_ = native_socket;
    impl.protocol_ = protocol;
    ec = lslboost::system::error_code();

    return ec;
  }

  // Bind the socket to the specified local endpoint.
  lslboost::system::error_code bind(implementation_type&,
      const endpoint_type&, lslboost::system::error_code& ec)
  {
    ec = lslboost::asio::error::operation_not_supported;
    return ec;
  }

  // Get the local endpoint.
  endpoint_type local_endpoint(const implementation_type& impl,
      lslboost::system::error_code& ec) const
  {
    endpoint_type endpoint;
    endpoint.resize(do_get_endpoint(impl, true,
          endpoint.data(), endpoint.size(), ec));
    return endpoint;
  }

  // Get the remote endpoint.
  endpoint_type remote_endpoint(const implementation_type& impl,
      lslboost::system::error_code& ec) const
  {
    endpoint_type endpoint;
    endpoint.resize(do_get_endpoint(impl, false,
          endpoint.data(), endpoint.size(), ec));
    return endpoint;
  }

  // Set a socket option.
  template <typename Option>
  lslboost::system::error_code set_option(implementation_type& impl,
      const Option& option, lslboost::system::error_code& ec)
  {
    return do_set_option(impl, option.level(impl.protocol_),
        option.name(impl.protocol_), option.data(impl.protocol_),
        option.size(impl.protocol_), ec);
  }

  // Get a socket option.
  template <typename Option>
  lslboost::system::error_code get_option(const implementation_type& impl,
      Option& option, lslboost::system::error_code& ec) const
  {
    std::size_t size = option.size(impl.protocol_);
    do_get_option(impl, option.level(impl.protocol_),
        option.name(impl.protocol_),
        option.data(impl.protocol_), &size, ec);
    if (!ec)
      option.resize(impl.protocol_, size);
    return ec;
  }

  // Connect the socket to the specified endpoint.
  lslboost::system::error_code connect(implementation_type& impl,
      const endpoint_type& peer_endpoint, lslboost::system::error_code& ec)
  {
    return do_connect(impl, peer_endpoint.data(), ec);
  }

  // Start an asynchronous connect.
  template <typename Handler>
  void async_connect(implementation_type& impl,
      const endpoint_type& peer_endpoint, Handler& handler)
  {
    bool is_continuation =
      lslboost_asio_handler_cont_helpers::is_continuation(handler);

    // Allocate and construct an operation to wrap the handler.
    typedef winrt_socket_connect_op<Handler> op;
    typename op::ptr p = { lslboost::asio::detail::addressof(handler),
      lslboost_asio_handler_alloc_helpers::allocate(
        sizeof(op), handler), 0 };
    p.p = new (p.v) op(handler);

    BOOST_ASIO_HANDLER_CREATION((p.p, "socket", &impl, "async_connect"));

    start_connect_op(impl, peer_endpoint.data(), p.p, is_continuation);
    p.v = p.p = 0;
  }
};

} // namespace detail
} // namespace asio
} // namespace lslboost

#include <lslboost/asio/detail/pop_options.hpp>

#endif // defined(BOOST_ASIO_WINDOWS_RUNTIME)

#endif // BOOST_ASIO_DETAIL_WINRT_SSOCKET_SERVICE_HPP
