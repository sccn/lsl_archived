//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_SYNC_UTILS_HPP
#define BOOST_INTERPROCESS_DETAIL_SYNC_UTILS_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>
#include <lslboost/interprocess/detail/win32_api.hpp>
#include <lslboost/interprocess/sync/spin/mutex.hpp>
#include <lslboost/interprocess/exceptions.hpp>
#include <lslboost/interprocess/sync/scoped_lock.hpp>
#include <lslboost/interprocess/sync/windows/winapi_semaphore_wrapper.hpp>
#include <lslboost/interprocess/sync/windows/winapi_mutex_wrapper.hpp>
#include <lslboost/unordered/unordered_map.hpp>
#include <cstddef>

namespace lslboost {
namespace interprocess {
namespace ipcdetail {

inline bool bytes_to_str(const void *mem, const std::size_t mem_length, char *out_str, std::size_t &out_length)
{
   const std::size_t need_mem = mem_length*2+1;
   if(out_length < need_mem){
      out_length = need_mem;
      return false;
   }

   const char Characters [] =
      { '0', '1', '2', '3', '4', '5', '6', '7'
      , '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

   std::size_t char_counter = 0;
   const char *buf = (const char *)mem;
   for(std::size_t i = 0; i != mem_length; ++i){
      out_str[char_counter++] = Characters[(buf[i]&0xF0)>>4];
      out_str[char_counter++] = Characters[(buf[i]&0x0F)];
   }
   out_str[char_counter] = 0;
   return true;
}

struct sync_id
{
   typedef __int64 internal_type;
   internal_type rand;

   sync_id()
   {  winapi::query_performance_counter(&rand);  }

   explicit sync_id(internal_type val)
   {  rand = val;  }

   friend std::size_t hash_value(const sync_id &m)
   {  return lslboost::hash_value(m.rand);  }

   friend bool operator==(const sync_id &l, const sync_id &r)
   {  return l.rand == r.rand;  }
};

class sync_handles
{
   public:
   enum type { MUTEX, SEMAPHORE };

   private:
   typedef lslboost::unordered_map<sync_id, void*> map_type;
   static const std::size_t LengthOfGlobal = sizeof("Global\\lslboost.ipc")-1;
   static const std::size_t StrSize        = LengthOfGlobal + (sizeof(sync_id)*2+1);
   typedef char NameBuf[StrSize];

   void fill_name(NameBuf &name, const sync_id &id)
   {
      const char *n = "Global\\lslboost.ipc";
      std::size_t i = 0;
      do{
         name[i] = n[i];
         ++i;
      } while(n[i]);
      std::size_t len = sizeof(NameBuf) - LengthOfGlobal;
      bytes_to_str(&id.rand, sizeof(id.rand), &name[LengthOfGlobal], len);
   }

   void erase_and_throw_if_error(void *hnd_val, const sync_id &id)
   {
      if(!hnd_val){
         map_.erase(id);
         error_info err(winapi::get_last_error());
         throw interprocess_exception(err);
      }
   }

   void* open_or_create_semaphore(const sync_id &id, unsigned int initial_count)
   {
      NameBuf name;
      fill_name(name, id);
      permissions unrestricted_security;
      unrestricted_security.set_unrestricted();
      winapi_semaphore_wrapper sem_wrapper;
      sem_wrapper.open_or_create(name, (long)initial_count, unrestricted_security);
      erase_and_throw_if_error(sem_wrapper.handle(), id);
      return sem_wrapper.release();
   }

   void* open_or_create_mutex(const sync_id &id)
   {
      NameBuf name;
      fill_name(name, id);
      permissions unrestricted_security;
      unrestricted_security.set_unrestricted();
      winapi_mutex_wrapper mtx_wrapper;
      mtx_wrapper.open_or_create(name, unrestricted_security);
      erase_and_throw_if_error(mtx_wrapper.handle(), id);
      return mtx_wrapper.release();
   }

   public:
   void *obtain_mutex(const sync_id &id, bool *popen_created = 0)
   {
      scoped_lock<spin_mutex> lock(mtx_);
      void *&hnd_val = map_[id];
      if(!hnd_val){
         hnd_val = open_or_create_mutex(id);
         if(popen_created) *popen_created = true;
      }
      return hnd_val;
   }

   void *obtain_semaphore(const sync_id &id, unsigned int initial_count, bool *popen_created = 0)
   {
      scoped_lock<spin_mutex> lock(mtx_);
      void *&hnd_val = map_[id];
      if(!hnd_val){
         hnd_val = open_or_create_semaphore(id, initial_count);
         if(popen_created) *popen_created = true;
      }
      return hnd_val;
   }

   void destroy_handle(const sync_id &id)
   {
      scoped_lock<spin_mutex> lock(mtx_);
      map_type::iterator it = map_.find(id);
      if(it != map_.end()){
         winapi::close_handle(it->second);
         map_.erase(it);
      }
   }

   private:
   spin_mutex mtx_;
   map_type map_;
};


}  //namespace ipcdetail {
}  //namespace interprocess {
}  //namespace lslboost {

#include <lslboost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_DETAIL_SYNC_UTILS_HPP
