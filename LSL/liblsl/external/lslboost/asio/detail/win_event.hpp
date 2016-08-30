//
// detail/win_event.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_WIN_EVENT_HPP
#define BOOST_ASIO_DETAIL_WIN_EVENT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_WINDOWS)

#include <lslboost/asio/detail/assert.hpp>
#include <lslboost/asio/detail/noncopyable.hpp>
#include <lslboost/asio/detail/socket_types.hpp>

#include <lslboost/asio/detail/push_options.hpp>

namespace lslboost {
namespace asio {
namespace detail {

class win_event
  : private noncopyable
{
public:
  // Constructor.
  BOOST_ASIO_DECL win_event();

  // Destructor.
  ~win_event()
  {
    ::CloseHandle(event_);
  }

  // Signal the event.
  template <typename Lock>
  void signal(Lock& lock)
  {
    BOOST_ASIO_ASSERT(lock.locked());
    (void)lock;
    ::SetEvent(event_);
  }

  // Signal the event and unlock the mutex.
  template <typename Lock>
  void signal_and_unlock(Lock& lock)
  {
    BOOST_ASIO_ASSERT(lock.locked());
    lock.unlock();
    ::SetEvent(event_);
  }

  // Reset the event.
  template <typename Lock>
  void clear(Lock& lock)
  {
    BOOST_ASIO_ASSERT(lock.locked());
    (void)lock;
    ::ResetEvent(event_);
  }

  // Wait for the event to become signalled.
  template <typename Lock>
  void wait(Lock& lock)
  {
    BOOST_ASIO_ASSERT(lock.locked());
    lock.unlock();
    ::WaitForSingleObject(event_, INFINITE);
    lock.lock();
  }

private:
  HANDLE event_;
};

} // namespace detail
} // namespace asio
} // namespace lslboost

#include <lslboost/asio/detail/pop_options.hpp>

#if defined(BOOST_ASIO_HEADER_ONLY)
# include <lslboost/asio/detail/impl/win_event.ipp>
#endif // defined(BOOST_ASIO_HEADER_ONLY)

#endif // defined(BOOST_ASIO_WINDOWS)

#endif // BOOST_ASIO_DETAIL_WIN_EVENT_HPP
