// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
// (C) Copyright 2009-2012 Anthony Williams
// (C) Copyright 2012 Vicente J. Botet Escriba

// Based on the Anthony's idea of thread_joiner in CCiA

#ifndef BOOST_THREAD_THREAD_GUARD_HPP
#define BOOST_THREAD_THREAD_GUARD_HPP

#include <lslboost/thread/detail/delete.hpp>
#include <lslboost/thread/detail/move.hpp>
#include <lslboost/thread/thread_functors.hpp>

#include <lslboost/config/abi_prefix.hpp>

namespace lslboost
{

  /**
   * Non-copyable RAII scoped thread guard joiner which join the thread if joinable when destroyed.
   */
  template <class CallableThread = join_if_joinable>
  class thread_guard
  {
    thread& t_;
  public:
    BOOST_THREAD_NO_COPYABLE( thread_guard)

    explicit thread_guard(thread& t) :
    t_(t)
    {
    }
    ~thread_guard()
    {
      CallableThread on_destructor;

      on_destructor(t_);
    }
  };

}
#include <lslboost/config/abi_suffix.hpp>

#endif
