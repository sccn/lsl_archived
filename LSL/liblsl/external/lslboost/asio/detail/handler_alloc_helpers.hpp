//
// detail/handler_alloc_helpers.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_HANDLER_ALLOC_HELPERS_HPP
#define BOOST_ASIO_DETAIL_HANDLER_ALLOC_HELPERS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <lslboost/asio/detail/config.hpp>
#include <lslboost/detail/workaround.hpp>
#include <lslboost/utility/addressof.hpp>
#include <lslboost/asio/detail/noncopyable.hpp>
#include <lslboost/asio/handler_alloc_hook.hpp>

#include <lslboost/asio/detail/push_options.hpp>

// Calls to asio_handler_allocate and asio_handler_deallocate must be made from
// a namespace that does not contain any overloads of these functions. The
// lslboost_asio_handler_alloc_helpers namespace is defined here for that purpose.
namespace lslboost_asio_handler_alloc_helpers {

template <typename Handler>
inline void* allocate(std::size_t s, Handler& h)
{
#if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564)) \
  || BOOST_WORKAROUND(__GNUC__, < 3)
  return ::operator new(s);
#else
  using lslboost::asio::asio_handler_allocate;
  return asio_handler_allocate(s, lslboost::addressof(h));
#endif
}

template <typename Handler>
inline void deallocate(void* p, std::size_t s, Handler& h)
{
#if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564)) \
  || BOOST_WORKAROUND(__GNUC__, < 3)
  ::operator delete(p);
#else
  using lslboost::asio::asio_handler_deallocate;
  asio_handler_deallocate(p, s, lslboost::addressof(h));
#endif
}

} // namespace lslboost_asio_handler_alloc_helpers

#define BOOST_ASIO_DEFINE_HANDLER_PTR(op) \
  struct ptr \
  { \
    Handler* h; \
    void* v; \
    op* p; \
    ~ptr() \
    { \
      reset(); \
    } \
    void reset() \
    { \
      if (p) \
      { \
        p->~op(); \
        p = 0; \
      } \
      if (v) \
      { \
        lslboost_asio_handler_alloc_helpers::deallocate(v, sizeof(op), *h); \
        v = 0; \
      } \
    } \
  } \
  /**/

#include <lslboost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_DETAIL_HANDLER_ALLOC_HELPERS_HPP
