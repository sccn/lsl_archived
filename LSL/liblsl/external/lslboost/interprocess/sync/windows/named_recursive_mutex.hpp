 //////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2011-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_INTERPROCESS_WINDOWS_RECURSIVE_NAMED_MUTEX_HPP
#define BOOST_INTERPROCESS_WINDOWS_RECURSIVE_NAMED_MUTEX_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>
#include <lslboost/interprocess/sync/windows/named_mutex.hpp>

namespace lslboost {
namespace interprocess {
namespace ipcdetail {


class windows_named_recursive_mutex
   //Windows mutexes based on CreateMutex are already recursive...
   : public windows_named_mutex
{
   /// @cond

   //Non-copyable
   windows_named_recursive_mutex();
   windows_named_recursive_mutex(const windows_named_mutex &);
   windows_named_recursive_mutex &operator=(const windows_named_mutex &);
   /// @endcond

   public:
   windows_named_recursive_mutex(create_only_t, const char *name, const permissions &perm = permissions())
      : windows_named_mutex(create_only_t(), name, perm)
   {}

   windows_named_recursive_mutex(open_or_create_t, const char *name, const permissions &perm = permissions())
      : windows_named_mutex(open_or_create_t(), name, perm)
   {}

   windows_named_recursive_mutex(open_only_t, const char *name)
      : windows_named_mutex(open_only_t(), name)
   {}
};

}  //namespace ipcdetail {
}  //namespace interprocess {
}  //namespace lslboost {

#include <lslboost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_WINDOWS_RECURSIVE_NAMED_MUTEX_HPP
