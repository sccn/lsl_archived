//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2009-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_XSI_SHARED_MEMORY_FILE_WRAPPER_HPP
#define BOOST_INTERPROCESS_XSI_SHARED_MEMORY_FILE_WRAPPER_HPP

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>
#include <lslboost/detail/workaround.hpp>

#if !defined(BOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS)
#error "This header can't be used in operating systems without XSI (System V) shared memory support"
#endif

#include <lslboost/interprocess/creation_tags.hpp>
#include <lslboost/interprocess/exceptions.hpp>
#include <lslboost/interprocess/detail/utilities.hpp>
#include <lslboost/interprocess/detail/os_file_functions.hpp>
#include <lslboost/interprocess/detail/tmp_dir_helpers.hpp>
#include <lslboost/interprocess/interprocess_fwd.hpp>
#include <lslboost/interprocess/exceptions.hpp>

#include <lslboost/interprocess/xsi_shared_memory.hpp>

//!\file
//!Describes a class representing a pseudo-file implemented on top of xsi shared memory.

namespace lslboost {
namespace interprocess {

class xsi_shared_memory_file_wrapper
   : public xsi_shared_memory
{
   /// @cond
   BOOST_MOVABLE_BUT_NOT_COPYABLE(xsi_shared_memory_file_wrapper)
   /// @endcond  
   public:

   xsi_shared_memory_file_wrapper() : xsi_shared_memory() {}

   xsi_shared_memory_file_wrapper(create_only_t, const xsi_key &key, mode_t mode, std::size_t size, const permissions& perm = permissions())
      : xsi_shared_memory(create_only_t(), key, size, perm.get_permissions())
   {}

   xsi_shared_memory_file_wrapper(open_or_create_t, const xsi_key &key, mode_t mode, std::size_t size, const permissions& perm = permissions())
      : xsi_shared_memory(open_or_create_t(), key, size, perm.get_permissions())
   {}

   xsi_shared_memory_file_wrapper(open_only_t, const xsi_key &key, mode_t mode, const permissions& perm = permissions())
      : xsi_shared_memory(open_only_t(), key)
   {}

   xsi_shared_memory_file_wrapper(BOOST_RV_REF(xsi_shared_memory_file_wrapper) moved)
   {  this->swap(moved);   }

   xsi_shared_memory_file_wrapper &operator=(BOOST_RV_REF(xsi_shared_memory_file_wrapper) moved)
   { 
      xsi_shared_memory_file_wrapper tmp(lslboost::move(moved));
      this->swap(tmp);
      return *this; 
   }

   //!Swaps two xsi_shared_memory_file_wrapper. Does not throw
   void swap(xsi_shared_memory_file_wrapper &other)
   {  this->xsi_shared_memory::swap(other);  }
};

}  //namespace interprocess {
}  //namespace lslboost {

#include <lslboost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_XSI_SHARED_MEMORY_FILE_WRAPPER_HPP
