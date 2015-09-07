//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINER_DETAIL_POOL_COMMON_ALLOC_HPP
#define BOOST_CONTAINER_DETAIL_POOL_COMMON_ALLOC_HPP

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <lslboost/container/detail/config_begin.hpp>
#include <lslboost/container/detail/workaround.hpp>
#include <lslboost/container/throw_exception.hpp>

#include <lslboost/intrusive/slist.hpp>
#include <lslboost/container/detail/pool_common.hpp>
#include <lslboost/container/detail/alloc_lib.h>
#include <cstddef>

namespace lslboost{
namespace container{
namespace container_detail{

struct node_slist_helper
   : public lslboost::container::container_detail::node_slist<void*>
{};

struct fake_segment_manager
{
   typedef void * void_pointer;
   static const std::size_t PayloadPerAllocation = BOOST_CONTAINER_ALLOCATION_PAYLOAD;

   typedef lslboost::container::container_detail::
      basic_multiallocation_chain<void*>              multiallocation_chain;
   static void deallocate(void_pointer p)
   { lslboost_cont_free(p); }

   static void deallocate_many(multiallocation_chain &chain)
   {
      std::size_t size = chain.size();
      std::pair<void*, void*> ptrs = chain.extract_data();
      lslboost_cont_memchain dlchain;
      BOOST_CONTAINER_MEMCHAIN_INIT_FROM(&dlchain, ptrs.first, ptrs.second, size);
      lslboost_cont_multidealloc(&dlchain);
   }

   typedef std::ptrdiff_t  difference_type;
   typedef std::size_t     size_type;

   static void *allocate_aligned(std::size_t nbytes, std::size_t alignment)
   {
      void *ret = lslboost_cont_memalign(nbytes, alignment);
      if(!ret)
         lslboost::container::throw_bad_alloc();
      return ret;
   }

   static void *allocate(std::size_t nbytes)
   {
      void *ret = lslboost_cont_malloc(nbytes);
      if(!ret)
         lslboost::container::throw_bad_alloc();
      return ret;
   }
};

}  //namespace lslboost{
}  //namespace container{
}  //namespace container_detail{

namespace lslboost {
namespace container {
namespace container_detail {

template<class T>
struct is_stateless_segment_manager;

template<>
struct is_stateless_segment_manager
   <lslboost::container::container_detail::fake_segment_manager>
{
   static const bool value = true;
};

}  //namespace container_detail {
}  //namespace container {
}  //namespace lslboost {

#include <lslboost/container/detail/config_end.hpp>

#endif   //BOOST_CONTAINER_DETAIL_POOL_COMMON_ALLOC_HPP
