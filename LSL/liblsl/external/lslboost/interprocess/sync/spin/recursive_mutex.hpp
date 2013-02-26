//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////
//
// Parts of the pthread code come from Boost Threads code:
//
//////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2001-2003
// William E. Kempf
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  William E. Kempf makes no representations
// about the suitability of this software for any purpose.
// It is provided "as is" without express or implied warranty.
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_SPIN_RECURSIVE_MUTEX_HPP
#define BOOST_INTERPROCESS_DETAIL_SPIN_RECURSIVE_MUTEX_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>

#include <lslboost/interprocess/detail/posix_time_types_wrk.hpp>
#include <lslboost/interprocess/detail/os_thread_functions.hpp>
#include <lslboost/interprocess/exceptions.hpp>
#include <lslboost/interprocess/detail/atomic.hpp>
#include <lslboost/cstdint.hpp>
#include <lslboost/interprocess/detail/os_thread_functions.hpp>
#include <lslboost/interprocess/sync/spin/mutex.hpp>
#include <lslboost/assert.hpp>

namespace lslboost {
namespace interprocess {
namespace ipcdetail {

class spin_recursive_mutex
{
   spin_recursive_mutex(const spin_recursive_mutex &);
   spin_recursive_mutex &operator=(const spin_recursive_mutex &);
   public:

   spin_recursive_mutex();
   ~spin_recursive_mutex();

   void lock();
   bool try_lock();
   bool timed_lock(const lslboost::posix_time::ptime &abs_time);
   void unlock();
   void take_ownership();
   private:
   spin_mutex     m_mutex;
   unsigned int   m_nLockCount;
   volatile ipcdetail::OS_systemwide_thread_id_t   m_nOwner;
   volatile lslboost::uint32_t m_s;
};

inline spin_recursive_mutex::spin_recursive_mutex()
   : m_nLockCount(0), m_nOwner(ipcdetail::get_invalid_systemwide_thread_id()){}

inline spin_recursive_mutex::~spin_recursive_mutex(){}

inline void spin_recursive_mutex::lock()
{
   typedef ipcdetail::OS_systemwide_thread_id_t handle_t;
   const handle_t thr_id(ipcdetail::get_current_systemwide_thread_id());
   handle_t old_id;
   ipcdetail::systemwide_thread_id_copy(m_nOwner, old_id);
   if(ipcdetail::equal_systemwide_thread_id(thr_id , old_id)){
      if((unsigned int)(m_nLockCount+1) == 0){
         //Overflow, throw an exception
         throw interprocess_exception("lslboost::interprocess::spin_recursive_mutex recursive lock overflow");
      }
      ++m_nLockCount;
   }
   else{
      m_mutex.lock();
      ipcdetail::systemwide_thread_id_copy(thr_id, m_nOwner);
      m_nLockCount = 1;
   }
}

inline bool spin_recursive_mutex::try_lock()
{
   typedef ipcdetail::OS_systemwide_thread_id_t handle_t;
   handle_t thr_id(ipcdetail::get_current_systemwide_thread_id());
   handle_t old_id;
   ipcdetail::systemwide_thread_id_copy(m_nOwner, old_id);
   if(ipcdetail::equal_systemwide_thread_id(thr_id , old_id)) {  // we own it
      if((unsigned int)(m_nLockCount+1) == 0){
         //Overflow, throw an exception
         throw interprocess_exception("lslboost::interprocess::spin_recursive_mutex recursive lock overflow");
      }
      ++m_nLockCount;
      return true;
   }
   if(m_mutex.try_lock()){
      ipcdetail::systemwide_thread_id_copy(thr_id, m_nOwner);
      m_nLockCount = 1;
      return true;
   }
   return false;
}

inline bool spin_recursive_mutex::timed_lock(const lslboost::posix_time::ptime &abs_time)
{
   typedef ipcdetail::OS_systemwide_thread_id_t handle_t;
   if(abs_time == lslboost::posix_time::pos_infin){
      this->lock();
      return true;
   }
   const handle_t thr_id(ipcdetail::get_current_systemwide_thread_id());
   handle_t old_id;
   ipcdetail::systemwide_thread_id_copy(m_nOwner, old_id);
   if(ipcdetail::equal_systemwide_thread_id(thr_id , old_id)) {  // we own it
      if((unsigned int)(m_nLockCount+1) == 0){
         //Overflow, throw an exception
         throw interprocess_exception("lslboost::interprocess::spin_recursive_mutex recursive lock overflow");
      }
      ++m_nLockCount;
      return true;
   }
   if(m_mutex.timed_lock(abs_time)){
      ipcdetail::systemwide_thread_id_copy(thr_id, m_nOwner);
      m_nLockCount = 1;
      return true;
   }
   return false;
}

inline void spin_recursive_mutex::unlock()
{
   typedef ipcdetail::OS_systemwide_thread_id_t handle_t;
   handle_t old_id;
   ipcdetail::systemwide_thread_id_copy(m_nOwner, old_id);
   const handle_t thr_id(ipcdetail::get_current_systemwide_thread_id());
   (void)old_id;
   (void)thr_id;
   BOOST_ASSERT(ipcdetail::equal_systemwide_thread_id(thr_id, old_id));
   --m_nLockCount;
   if(!m_nLockCount){
      const handle_t new_id(ipcdetail::get_invalid_systemwide_thread_id());
      ipcdetail::systemwide_thread_id_copy(new_id, m_nOwner);
      m_mutex.unlock();
   }
}

inline void spin_recursive_mutex::take_ownership()
{
   typedef ipcdetail::OS_systemwide_thread_id_t handle_t;
   this->m_nLockCount = 1;
   const handle_t thr_id(ipcdetail::get_current_systemwide_thread_id());
   ipcdetail::systemwide_thread_id_copy(thr_id, m_nOwner);
}

}  //namespace ipcdetail {
}  //namespace interprocess {
}  //namespace lslboost {

#include <lslboost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_DETAIL_SPIN_RECURSIVE_MUTEX_HPP
