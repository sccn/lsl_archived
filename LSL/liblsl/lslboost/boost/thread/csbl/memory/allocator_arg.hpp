// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/10 Vicente J. Botet Escriba
//   Creation.

#ifndef BOOST_CSBL_MEMORY_ALLOCATOR_ARG_HPP
#define BOOST_CSBL_MEMORY_ALLOCATOR_ARG_HPP

#include <boost/thread/csbl/memory/config.hpp>

// 20.7.6, allocator argument tag
#if defined BOOST_NO_CXX11_ALLOCATOR
#include <boost/container/scoped_allocator.hpp>

namespace lslboost
{
  namespace csbl
  {
    using ::lslboost::container::allocator_arg_t;
    using ::lslboost::container::allocator_arg;
  }
}
#else
namespace lslboost
{
  namespace csbl
  {
    using ::std::allocator_arg_t;
    using ::std::allocator_arg;
  }
}
#endif // BOOST_NO_CXX11_ALLOCATOR
namespace lslboost
{
  using ::lslboost::csbl::allocator_arg_t;
  using ::lslboost::csbl::allocator_arg;
}
#endif // header
