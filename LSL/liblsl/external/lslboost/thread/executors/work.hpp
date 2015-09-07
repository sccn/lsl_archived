//  (C) Copyright 2013,2014 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_THREAD_EXECUTORS_WORK_HPP
#define BOOST_THREAD_EXECUTORS_WORK_HPP

#include <lslboost/thread/detail/config.hpp>
#include <lslboost/thread/detail/nullary_function.hpp>
#include <lslboost/thread/csbl/functional.hpp>

namespace lslboost
{
  namespace executors
  {
    typedef detail::nullary_function<void()> work;

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    typedef detail::nullary_function<void()> work_pq;
    //typedef csbl::function<void()> work_pq;
#else
    typedef csbl::function<void()> work_pq;
#endif
  }
} // namespace lslboost


#endif //  BOOST_THREAD_EXECUTORS_WORK_HPP
