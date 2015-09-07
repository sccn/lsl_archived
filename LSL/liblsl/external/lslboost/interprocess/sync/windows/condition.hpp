//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_WINDOWS_CONDITION_HPP
#define BOOST_INTERPROCESS_DETAIL_WINDOWS_CONDITION_HPP

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif
#
#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>
#include <lslboost/interprocess/detail/posix_time_types_wrk.hpp>

#include <lslboost/interprocess/sync/interprocess_mutex.hpp>
#include <lslboost/interprocess/sync/scoped_lock.hpp>
#include <lslboost/interprocess/exceptions.hpp>
#include <lslboost/interprocess/sync/windows/semaphore.hpp>
#include <lslboost/interprocess/sync/windows/mutex.hpp>
#include <lslboost/interprocess/sync/detail/condition_algorithm_8a.hpp>


namespace lslboost {
namespace interprocess {
namespace ipcdetail {

class windows_condition
{
   windows_condition(const windows_condition &);
   windows_condition &operator=(const windows_condition &);

   public:
   windows_condition()
      : m_condition_data()
   {}

   ~windows_condition()
   {
      //Notify all waiting threads
      //to allow POSIX semantics on condition destruction
      this->notify_all();
   }

   void notify_one()
   {  m_condition_data.notify_one();   }

   void notify_all()
   {  m_condition_data.notify_all();   }

   template <typename L>
   bool timed_wait(L& lock, const lslboost::posix_time::ptime &abs_time)
   {  return m_condition_data.timed_wait(lock, abs_time);   }

   template <typename L, typename Pr>
   bool timed_wait(L& lock, const lslboost::posix_time::ptime &abs_time, Pr pred)
   {  return m_condition_data.timed_wait(lock, abs_time, pred);   }

   template <typename L>
   void wait(L& lock)
   {  m_condition_data.wait(lock);   }

   template <typename L, typename Pr>
   void wait(L& lock, Pr pred)
   {  m_condition_data.wait(lock, pred);   }

   private:

   struct condition_data
   {
      typedef lslboost::int32_t     integer_type;
      typedef windows_semaphore  semaphore_type;
      typedef windows_mutex      mutex_type;

      condition_data()
         : m_nwaiters_blocked(0)
         , m_nwaiters_gone(0)
         , m_nwaiters_to_unblock(0)
         , m_sem_block_queue(0)
         , m_sem_block_lock(1)
         , m_mtx_unblock_lock()
      {}

      integer_type    &get_nwaiters_blocked()
      {  return m_nwaiters_blocked;  }

      integer_type    &get_nwaiters_gone()
      {  return m_nwaiters_gone;  }

      integer_type    &get_nwaiters_to_unblock()
      {  return m_nwaiters_to_unblock;  }

      semaphore_type  &get_sem_block_queue()
      {  return m_sem_block_queue;  }

      semaphore_type  &get_sem_block_lock()
      {  return m_sem_block_lock;  }

      mutex_type      &get_mtx_unblock_lock()
      {  return m_mtx_unblock_lock;  }

      lslboost::int32_t    m_nwaiters_blocked;
      lslboost::int32_t    m_nwaiters_gone;
      lslboost::int32_t    m_nwaiters_to_unblock;
      windows_semaphore m_sem_block_queue;
      windows_semaphore m_sem_block_lock;
      windows_mutex     m_mtx_unblock_lock;
   };

   ipcdetail::condition_8a_wrapper<condition_data> m_condition_data;
};

}  //namespace ipcdetail
}  //namespace interprocess
}  //namespace lslboost

#include <lslboost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_DETAIL_WINDOWS_CONDITION_HPP
