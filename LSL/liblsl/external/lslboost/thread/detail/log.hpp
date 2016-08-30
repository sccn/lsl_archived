// Copyright (C) 2012 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_THREAD_DETAIL_LOG_HPP
#define BOOST_THREAD_DETAIL_LOG_HPP

#include <lslboost/thread/detail/config.hpp>
#if defined BOOST_THREAD_USES_LOG
#include <lslboost/thread/recursive_mutex.hpp>
#include <lslboost/thread/lock_guard.hpp>
#if defined BOOST_THREAD_USES_LOG_THREAD_ID
#include <lslboost/thread/thread.hpp>
#endif
#include <iostream>

namespace lslboost
{
  namespace thread_detail
  {
    inline lslboost::recursive_mutex& terminal_mutex()
    {
      static lslboost::recursive_mutex mtx;
      return mtx;
    }

  }
}
#if defined BOOST_THREAD_USES_LOG_THREAD_ID

#define BOOST_THREAD_LOG \
  { \
    lslboost::lock_guard<lslboost::recursive_mutex> _lk_(lslboost::thread_detail::terminal_mutex()); \
    std::cout << lslboost::this_thread::get_id() << " - "<<__FILE__<<"["<<__LINE__<<"] " <<std::dec
#else

#define BOOST_THREAD_LOG \
{ \
  lslboost::lock_guard<lslboost::recursive_mutex> _lk_(lslboost::thread_detail::terminal_mutex()); \
  std::cout << __FILE__<<"["<<__LINE__<<"] " <<std::dec

#endif
#define BOOST_THREAD_END_LOG \
    std::dec << std::endl; \
  }

#else

namespace lslboost
{
  namespace thread_detail
  {
    struct dummy_stream_t
    {
    };

    template <typename T>
    inline dummy_stream_t const& operator<<(dummy_stream_t const& os, T)
    {
      return os;
    }

    inline dummy_stream_t const& operator<<(dummy_stream_t const& os, dummy_stream_t const&)
    {
      return os;
    }


    BOOST_CONSTEXPR_OR_CONST dummy_stream_t dummy_stream = {};

  }
}

#define BOOST_THREAD_LOG if (true) {} else lslboost::thread_detail::dummy_stream
#define BOOST_THREAD_END_LOG lslboost::thread_detail::dummy_stream

#endif

#define BOOST_THREAD_TRACE BOOST_THREAD_LOG << BOOST_THREAD_END_LOG


#endif // header
