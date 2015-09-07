// Copyright (C) 2013 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// 2013/10 Vicente J. Botet Escriba
//   Creation.

#ifndef BOOST_CSBL_MEMORY_DEFAULT_DELETE_HPP
#define BOOST_CSBL_MEMORY_DEFAULT_DELETE_HPP

#include <lslboost/thread/csbl/memory/config.hpp>

// 20.8.1 class template unique_ptr:
// default_delete

#if defined BOOST_NO_CXX11_SMART_PTR
#include <lslboost/move/unique_ptr.hpp>

namespace lslboost
{
  namespace csbl
  {
    using ::lslboost::movelib::default_delete;
  }
}
#else
namespace lslboost
{
  namespace csbl
  {
    using ::std::default_delete;
  }
}
#endif // defined  BOOST_NO_CXX11_SMART_PTR

namespace lslboost
{
  using ::lslboost::csbl::default_delete;
}
#endif // header
