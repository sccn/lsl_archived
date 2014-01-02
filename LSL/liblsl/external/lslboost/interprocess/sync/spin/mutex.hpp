//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_SPIN_MUTEX_HPP
#define BOOST_INTERPROCESS_DETAIL_SPIN_MUTEX_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>
#include <lslboost/interprocess/detail/posix_time_types_wrk.hpp>
#include <lslboost/assert.hpp>
#include <lslboost/interprocess/detail/atomic.hpp>
#include <lslboost/cstdint.hpp>
#include <lslboost/interprocess/detail/os_thread_functions.hpp>

namespace lslboost {
namespace interprocess {
namespace ipcdetail {

class spin_mutex
{
   spin_mutex(const spin_mutex &);
   spin_mutex &operator=(const spin_mutex &);
   public:

   spin_mutex();
   ~spin_mutex();

   void lock();
   bool try_lock();
   bool timed_lock(const lslboost::posix_time::ptime &abs_time);
   void unlock();
   void take_ownership(){};
   private:
   volatile lslboost::uint32_t m_s;
};

inline spin_mutex::spin_mutex()
   : m_s(0)
{
   //Note that this class is initialized to zero.
   //So zeroed memory can be interpreted as an
   //initialized mutex
}

inline spin_mutex::~spin_mutex()
{
   //Trivial destructor
}

inline void spin_mutex::lock(void)
{
   do{
      lslboost::uint32_t prev_s = ipcdetail::atomic_cas32(const_cast<lslboost::uint32_t*>(&m_s), 1, 0);

      if (m_s == 1 && prev_s == 0){
            break;
      }
      // relinquish current timeslice
      ipcdetail::thread_yield();
   }while (true);
}

inline bool spin_mutex::try_lock(void)
{
   lslboost::uint32_t prev_s = ipcdetail::atomic_cas32(const_cast<lslboost::uint32_t*>(&m_s), 1, 0);  
   return m_s == 1 && prev_s == 0;
}

inline bool spin_mutex::timed_lock(const lslboost::posix_time::ptime &abs_time)
{
   if(abs_time == lslboost::posix_time::pos_infin){
      this->lock();
      return true;
   }
   //Obtain current count and target time
   lslboost::posix_time::ptime now = microsec_clock::universal_time();

   do{
      if(this->try_lock()){
         break;
      }
      now = microsec_clock::universal_time();

      if(now >= abs_time){
         return false;
      }
      // relinquish current time slice
     ipcdetail::thread_yield();
   }while (true);

   return true;
}

inline void spin_mutex::unlock(void)
{  ipcdetail::atomic_cas32(const_cast<lslboost::uint32_t*>(&m_s), 0, 1);   }

}  //namespace ipcdetail {
}  //namespace interprocess {
}  //namespace lslboost {

#include <lslboost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_DETAIL_SPIN_MUTEX_HPP
