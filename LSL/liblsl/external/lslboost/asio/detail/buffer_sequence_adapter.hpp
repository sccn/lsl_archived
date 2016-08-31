//
// detail/buffer_sequence_adapter.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_BUFFER_SEQUENCE_ADAPTER_HPP
#define BOOST_ASIO_DETAIL_BUFFER_SEQUENCE_ADAPTER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>
#include <lslboost/asio/buffer.hpp>
#include <lslboost/asio/detail/array_fwd.hpp>
#include <lslboost/asio/detail/socket_types.hpp>

#include <lslboost/asio/detail/push_options.hpp>

namespace lslboost {
namespace asio {
namespace detail {

class buffer_sequence_adapter_base
{
protected:
#if defined(BOOST_ASIO_WINDOWS_RUNTIME)
  // The maximum number of buffers to support in a single operation.
  enum { max_buffers = 1 };

  typedef Windows::Storage::Streams::IBuffer^ native_buffer_type;

  BOOST_ASIO_DECL static void init_native_buffer(
      native_buffer_type& buf,
      const lslboost::asio::mutable_buffer& buffer);

  BOOST_ASIO_DECL static void init_native_buffer(
      native_buffer_type& buf,
      const lslboost::asio::const_buffer& buffer);
#elif defined(BOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
  // The maximum number of buffers to support in a single operation.
  enum { max_buffers = 64 < max_iov_len ? 64 : max_iov_len };

  typedef WSABUF native_buffer_type;

  static void init_native_buffer(WSABUF& buf,
      const lslboost::asio::mutable_buffer& buffer)
  {
    buf.buf = lslboost::asio::buffer_cast<char*>(buffer);
    buf.len = static_cast<ULONG>(lslboost::asio::buffer_size(buffer));
  }

  static void init_native_buffer(WSABUF& buf,
      const lslboost::asio::const_buffer& buffer)
  {
    buf.buf = const_cast<char*>(lslboost::asio::buffer_cast<const char*>(buffer));
    buf.len = static_cast<ULONG>(lslboost::asio::buffer_size(buffer));
  }
#else // defined(BOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
  // The maximum number of buffers to support in a single operation.
  enum { max_buffers = 64 < max_iov_len ? 64 : max_iov_len };

  typedef iovec native_buffer_type;

  static void init_iov_base(void*& base, void* addr)
  {
    base = addr;
  }

  template <typename T>
  static void init_iov_base(T& base, void* addr)
  {
    base = static_cast<T>(addr);
  }

  static void init_native_buffer(iovec& iov,
      const lslboost::asio::mutable_buffer& buffer)
  {
    init_iov_base(iov.iov_base, lslboost::asio::buffer_cast<void*>(buffer));
    iov.iov_len = lslboost::asio::buffer_size(buffer);
  }

  static void init_native_buffer(iovec& iov,
      const lslboost::asio::const_buffer& buffer)
  {
    init_iov_base(iov.iov_base, const_cast<void*>(
          lslboost::asio::buffer_cast<const void*>(buffer)));
    iov.iov_len = lslboost::asio::buffer_size(buffer);
  }
#endif // defined(BOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
};

// Helper class to translate buffers into the native buffer representation.
template <typename Buffer, typename Buffers>
class buffer_sequence_adapter
  : buffer_sequence_adapter_base
{
public:
  explicit buffer_sequence_adapter(const Buffers& buffer_sequence)
    : count_(0), total_buffer_size_(0)
  {
    typename Buffers::const_iterator iter = buffer_sequence.begin();
    typename Buffers::const_iterator end = buffer_sequence.end();
    for (; iter != end && count_ < max_buffers; ++iter, ++count_)
    {
      Buffer buffer(*iter);
      init_native_buffer(buffers_[count_], buffer);
      total_buffer_size_ += lslboost::asio::buffer_size(buffer);
    }
  }

  native_buffer_type* buffers()
  {
    return buffers_;
  }

  std::size_t count() const
  {
    return count_;
  }

  bool all_empty() const
  {
    return total_buffer_size_ == 0;
  }

  static bool all_empty(const Buffers& buffer_sequence)
  {
    typename Buffers::const_iterator iter = buffer_sequence.begin();
    typename Buffers::const_iterator end = buffer_sequence.end();
    std::size_t i = 0;
    for (; iter != end && i < max_buffers; ++iter, ++i)
      if (lslboost::asio::buffer_size(Buffer(*iter)) > 0)
        return false;
    return true;
  }

  static void validate(const Buffers& buffer_sequence)
  {
    typename Buffers::const_iterator iter = buffer_sequence.begin();
    typename Buffers::const_iterator end = buffer_sequence.end();
    for (; iter != end; ++iter)
    {
      Buffer buffer(*iter);
      lslboost::asio::buffer_cast<const void*>(buffer);
    }
  }

  static Buffer first(const Buffers& buffer_sequence)
  {
    typename Buffers::const_iterator iter = buffer_sequence.begin();
    typename Buffers::const_iterator end = buffer_sequence.end();
    for (; iter != end; ++iter)
    {
      Buffer buffer(*iter);
      if (lslboost::asio::buffer_size(buffer) != 0)
        return buffer;
    }
    return Buffer();
  }

private:
  native_buffer_type buffers_[max_buffers];
  std::size_t count_;
  std::size_t total_buffer_size_;
};

template <typename Buffer>
class buffer_sequence_adapter<Buffer, lslboost::asio::mutable_buffers_1>
  : buffer_sequence_adapter_base
{
public:
  explicit buffer_sequence_adapter(
      const lslboost::asio::mutable_buffers_1& buffer_sequence)
  {
    init_native_buffer(buffer_, Buffer(buffer_sequence));
    total_buffer_size_ = lslboost::asio::buffer_size(buffer_sequence);
  }

  native_buffer_type* buffers()
  {
    return &buffer_;
  }

  std::size_t count() const
  {
    return 1;
  }

  bool all_empty() const
  {
    return total_buffer_size_ == 0;
  }

  static bool all_empty(const lslboost::asio::mutable_buffers_1& buffer_sequence)
  {
    return lslboost::asio::buffer_size(buffer_sequence) == 0;
  }

  static void validate(const lslboost::asio::mutable_buffers_1& buffer_sequence)
  {
    lslboost::asio::buffer_cast<const void*>(buffer_sequence);
  }

  static Buffer first(const lslboost::asio::mutable_buffers_1& buffer_sequence)
  {
    return Buffer(buffer_sequence);
  }

private:
  native_buffer_type buffer_;
  std::size_t total_buffer_size_;
};

template <typename Buffer>
class buffer_sequence_adapter<Buffer, lslboost::asio::const_buffers_1>
  : buffer_sequence_adapter_base
{
public:
  explicit buffer_sequence_adapter(
      const lslboost::asio::const_buffers_1& buffer_sequence)
  {
    init_native_buffer(buffer_, Buffer(buffer_sequence));
    total_buffer_size_ = lslboost::asio::buffer_size(buffer_sequence);
  }

  native_buffer_type* buffers()
  {
    return &buffer_;
  }

  std::size_t count() const
  {
    return 1;
  }

  bool all_empty() const
  {
    return total_buffer_size_ == 0;
  }

  static bool all_empty(const lslboost::asio::const_buffers_1& buffer_sequence)
  {
    return lslboost::asio::buffer_size(buffer_sequence) == 0;
  }

  static void validate(const lslboost::asio::const_buffers_1& buffer_sequence)
  {
    lslboost::asio::buffer_cast<const void*>(buffer_sequence);
  }

  static Buffer first(const lslboost::asio::const_buffers_1& buffer_sequence)
  {
    return Buffer(buffer_sequence);
  }

private:
  native_buffer_type buffer_;
  std::size_t total_buffer_size_;
};

template <typename Buffer, typename Elem>
class buffer_sequence_adapter<Buffer, lslboost::array<Elem, 2> >
  : buffer_sequence_adapter_base
{
public:
  explicit buffer_sequence_adapter(
      const lslboost::array<Elem, 2>& buffer_sequence)
  {
    init_native_buffer(buffers_[0], Buffer(buffer_sequence[0]));
    init_native_buffer(buffers_[1], Buffer(buffer_sequence[1]));
    total_buffer_size_ = lslboost::asio::buffer_size(buffer_sequence[0])
      + lslboost::asio::buffer_size(buffer_sequence[1]);
  }

  native_buffer_type* buffers()
  {
    return buffers_;
  }

  std::size_t count() const
  {
    return 2;
  }

  bool all_empty() const
  {
    return total_buffer_size_ == 0;
  }

  static bool all_empty(const lslboost::array<Elem, 2>& buffer_sequence)
  {
    return lslboost::asio::buffer_size(buffer_sequence[0]) == 0
      && lslboost::asio::buffer_size(buffer_sequence[1]) == 0;
  }

  static void validate(const lslboost::array<Elem, 2>& buffer_sequence)
  {
    lslboost::asio::buffer_cast<const void*>(buffer_sequence[0]);
    lslboost::asio::buffer_cast<const void*>(buffer_sequence[1]);
  }

  static Buffer first(const lslboost::array<Elem, 2>& buffer_sequence)
  {
    return Buffer(lslboost::asio::buffer_size(buffer_sequence[0]) != 0
        ? buffer_sequence[0] : buffer_sequence[1]);
  }

private:
  native_buffer_type buffers_[2];
  std::size_t total_buffer_size_;
};

#if defined(BOOST_ASIO_HAS_STD_ARRAY)

template <typename Buffer, typename Elem>
class buffer_sequence_adapter<Buffer, std::array<Elem, 2> >
  : buffer_sequence_adapter_base
{
public:
  explicit buffer_sequence_adapter(
      const std::array<Elem, 2>& buffer_sequence)
  {
    init_native_buffer(buffers_[0], Buffer(buffer_sequence[0]));
    init_native_buffer(buffers_[1], Buffer(buffer_sequence[1]));
    total_buffer_size_ = lslboost::asio::buffer_size(buffer_sequence[0])
      + lslboost::asio::buffer_size(buffer_sequence[1]);
  }

  native_buffer_type* buffers()
  {
    return buffers_;
  }

  std::size_t count() const
  {
    return 2;
  }

  bool all_empty() const
  {
    return total_buffer_size_ == 0;
  }

  static bool all_empty(const std::array<Elem, 2>& buffer_sequence)
  {
    return lslboost::asio::buffer_size(buffer_sequence[0]) == 0
      && lslboost::asio::buffer_size(buffer_sequence[1]) == 0;
  }

  static void validate(const std::array<Elem, 2>& buffer_sequence)
  {
    lslboost::asio::buffer_cast<const void*>(buffer_sequence[0]);
    lslboost::asio::buffer_cast<const void*>(buffer_sequence[1]);
  }

  static Buffer first(const std::array<Elem, 2>& buffer_sequence)
  {
    return Buffer(lslboost::asio::buffer_size(buffer_sequence[0]) != 0
        ? buffer_sequence[0] : buffer_sequence[1]);
  }

private:
  native_buffer_type buffers_[2];
  std::size_t total_buffer_size_;
};

#endif // defined(BOOST_ASIO_HAS_STD_ARRAY)

} // namespace detail
} // namespace asio
} // namespace lslboost

#include <lslboost/asio/detail/pop_options.hpp>

#if defined(BOOST_ASIO_HEADER_ONLY)
# include <lslboost/asio/detail/impl/buffer_sequence_adapter.ipp>
#endif // defined(BOOST_ASIO_HEADER_ONLY)

#endif // BOOST_ASIO_DETAIL_BUFFER_SEQUENCE_ADAPTER_HPP
