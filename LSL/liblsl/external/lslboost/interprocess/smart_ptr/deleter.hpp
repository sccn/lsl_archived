//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2011.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DELETER_HPP
#define BOOST_INTERPROCESS_DELETER_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/interprocess_fwd.hpp>
#include <lslboost/interprocess/detail/utilities.hpp>
#include <lslboost/intrusive/pointer_traits.hpp>

//!\file
//!Describes the functor to delete objects from the segment.

namespace lslboost {
namespace interprocess {

//!A deleter that uses the segment manager's destroy_ptr
//!function to destroy the passed pointer resource.
//!
//!This deleter is used
template<class T, class SegmentManager>
class deleter
{
   public:
   typedef typename lslboost::intrusive::
      pointer_traits<typename SegmentManager::void_pointer>::template
         rebind_pointer<T>::type                pointer;

   private:
   typedef typename lslboost::intrusive::
      pointer_traits<pointer>::template
         rebind_pointer<SegmentManager>::type                segment_manager_pointer;

   segment_manager_pointer mp_mngr;

   public:
   deleter(segment_manager_pointer pmngr)
      :  mp_mngr(pmngr)
   {}

   void operator()(const pointer &p)
   {  mp_mngr->destroy_ptr(ipcdetail::to_raw_pointer(p));   }
};

}  //namespace interprocess {
}  //namespace lslboost {

#include <lslboost/interprocess/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTERPROCESS_DELETER_HPP
