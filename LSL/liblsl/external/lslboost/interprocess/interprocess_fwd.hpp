//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_FWD_HPP
#define BOOST_INTERPROCESS_FWD_HPP

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif
#
#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <lslboost/interprocess/detail/std_fwd.hpp>

//! \file
//! This header file forward declares the basic interprocess types:
//!   - lslboost::interprocess::offset_ptr;
//!   - lslboost::interprocess::permissions;
//!   - lslboost::interprocess::mapped_region;
//!   - lslboost::interprocess::file_mapping;
//!   - lslboost::interprocess::shared_memory_object;
//!   - lslboost::interprocess::windows_shared_memory;
//!   - lslboost::interprocess::xsi_shared_memory;
//!
//! The following synchronization mechanisms and locks:
//!   - lslboost::interprocess::null_mutex;
//!   - lslboost::interprocess::interprocess_mutex;
//!   - lslboost::interprocess::interprocess_recursive_mutex;
//!   - lslboost::interprocess::interprocess_semaphore;
//!   - lslboost::interprocess::named_mutex;
//!   - lslboost::interprocess::named_recursive_mutex;
//!   - lslboost::interprocess::named_semaphore;
//!   - lslboost::interprocess::interprocess_sharable_mutex;
//!   - lslboost::interprocess::interprocess_condition;
//!   - lslboost::interprocess::scoped_lock;
//!   - lslboost::interprocess::sharable_lock;
//!   - lslboost::interprocess::upgradable_lock;
//!
//! The following mutex families:
//!   - lslboost::interprocess::mutex_family;
//!   - lslboost::interprocess::null_mutex_family;
//!
//! The following allocators:
//!   - lslboost::interprocess::allocator;
//!   - lslboost::interprocess::node_allocator;
//!   - lslboost::interprocess::private_node_allocator;
//!   - lslboost::interprocess::cached_node_allocator;
//!   - lslboost::interprocess::adaptive_pool;
//!   - lslboost::interprocess::private_adaptive_pool;
//!   - lslboost::interprocess::cached_adaptive_pool;
//!
//! The following allocation algorithms:
//!   - lslboost::interprocess::simple_seq_fit;
//!   - lslboost::interprocess::rbtree_best_fit;
//!
//! The following index types:
//!   - lslboost::interprocess::flat_map_index;
//!   - lslboost::interprocess::iset_index;
//!   - lslboost::interprocess::iunordered_set_index;
//!   - lslboost::interprocess::map_index;
//!   - lslboost::interprocess::null_index;
//!   - lslboost::interprocess::unordered_map_index;
//!
//! The following managed memory types:
//!   - lslboost::interprocess::segment_manager;
//!   - lslboost::interprocess::basic_managed_external_buffer
//!   - lslboost::interprocess::managed_external_buffer
//!   - lslboost::interprocess::wmanaged_external_buffer
//!   - lslboost::interprocess::basic_managed_shared_memory
//!   - lslboost::interprocess::managed_shared_memory
//!   - lslboost::interprocess::wmanaged_shared_memory
//!   - lslboost::interprocess::basic_managed_windows_shared_memory
//!   - lslboost::interprocess::managed_windows_shared_memory
//!   - lslboost::interprocess::wmanaged_windows_shared_memory
//!   - lslboost::interprocess::basic_managed_xsi_shared_memory
//!   - lslboost::interprocess::managed_xsi_shared_memory
//!   - lslboost::interprocess::wmanaged_xsi_shared_memory
//!   - lslboost::interprocess::fixed_managed_shared_memory
//!   - lslboost::interprocess::wfixed_managed_shared_memory
//!   - lslboost::interprocess::basic_managed_heap_memory
//!   - lslboost::interprocess::managed_heap_memory
//!   - lslboost::interprocess::wmanaged_heap_memory
//!   - lslboost::interprocess::basic_managed_mapped_file
//!   - lslboost::interprocess::managed_mapped_file
//!   - lslboost::interprocess::wmanaged_mapped_file
//!
//! The following exception types:
//!   - lslboost::interprocess::interprocess_exception
//!   - lslboost::interprocess::lock_exception
//!   - lslboost::interprocess::bad_alloc
//!
//! The following stream types:
//!   - lslboost::interprocess::basic_bufferbuf
//!   - lslboost::interprocess::basic_ibufferstream
//!   - lslboost::interprocess::basic_obufferstream
//!   - lslboost::interprocess::basic_bufferstream
//!   - lslboost::interprocess::basic_vectorbuf
//!   - lslboost::interprocess::basic_ivectorstream
//!   - lslboost::interprocess::basic_ovectorstream
//!   - lslboost::interprocess::basic_vectorstream
//!
//! The following smart pointer types:
//!   - lslboost::interprocess::scoped_ptr
//!   - lslboost::interprocess::intrusive_ptr
//!   - lslboost::interprocess::shared_ptr
//!   - lslboost::interprocess::weak_ptr
//!
//! The following interprocess communication types:
//!   - lslboost::interprocess::message_queue_t;
//!   - lslboost::interprocess::message_queue;

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>

#if !defined(BOOST_INTERPROCESS_DOXYGEN_INVOKED)

#include <cstddef>

//////////////////////////////////////////////////////////////////////////////
//                        Standard predeclarations
//////////////////////////////////////////////////////////////////////////////

namespace lslboost{  namespace intrusive{ }  }
namespace lslboost{  namespace interprocess{ namespace bi = lslboost::intrusive; }  }

namespace lslboost { namespace interprocess {

//////////////////////////////////////////////////////////////////////////////
//                            permissions
//////////////////////////////////////////////////////////////////////////////

class permissions;

//////////////////////////////////////////////////////////////////////////////
//                            shared_memory
//////////////////////////////////////////////////////////////////////////////

class shared_memory_object;

#if defined (BOOST_INTERPROCESS_WINDOWS)
class windows_shared_memory;
#endif   //#if defined (BOOST_INTERPROCESS_WINDOWS)

#if defined(BOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS)
class xsi_shared_memory;
#endif   //#if defined (BOOST_INTERPROCESS_WINDOWS)

//////////////////////////////////////////////////////////////////////////////
//              file mapping / mapped region
//////////////////////////////////////////////////////////////////////////////

class file_mapping;
class mapped_region;

//////////////////////////////////////////////////////////////////////////////
//                               Mutexes
//////////////////////////////////////////////////////////////////////////////

class null_mutex;

class interprocess_mutex;
class interprocess_recursive_mutex;

class named_mutex;
class named_recursive_mutex;

class interprocess_semaphore;
class named_semaphore;

//////////////////////////////////////////////////////////////////////////////
//                         Mutex families
//////////////////////////////////////////////////////////////////////////////

struct mutex_family;
struct null_mutex_family;

//////////////////////////////////////////////////////////////////////////////
//                   Other synchronization classes
//////////////////////////////////////////////////////////////////////////////

class interprocess_sharable_mutex;
class interprocess_condition;

//////////////////////////////////////////////////////////////////////////////
//                              Locks
//////////////////////////////////////////////////////////////////////////////

template <class Mutex>
class scoped_lock;

template <class SharableMutex>
class sharable_lock;

template <class UpgradableMutex>
class upgradable_lock;

//////////////////////////////////////////////////////////////////////////////
//                      STL compatible allocators
//////////////////////////////////////////////////////////////////////////////

template<class T, class SegmentManager>
class allocator;

template<class T, class SegmentManager, std::size_t NodesPerBlock = 64>
class node_allocator;

template<class T, class SegmentManager, std::size_t NodesPerBlock = 64>
class private_node_allocator;

template<class T, class SegmentManager, std::size_t NodesPerBlock = 64>
class cached_node_allocator;

template< class T, class SegmentManager, std::size_t NodesPerBlock = 64
        , std::size_t MaxFreeBlocks = 2, unsigned char OverheadPercent = 5 >
class adaptive_pool;

template< class T, class SegmentManager, std::size_t NodesPerBlock = 64
        , std::size_t MaxFreeBlocks = 2, unsigned char OverheadPercent = 5 >
class private_adaptive_pool;

template< class T, class SegmentManager, std::size_t NodesPerBlock = 64
        , std::size_t MaxFreeBlocks = 2, unsigned char OverheadPercent = 5 >
class cached_adaptive_pool;


//////////////////////////////////////////////////////////////////////////////
//                            offset_ptr
//////////////////////////////////////////////////////////////////////////////

static const std::size_t offset_type_alignment = 0;

template < class T, class DifferenceType = std::ptrdiff_t
         , class OffsetType = std::size_t, std::size_t Alignment = offset_type_alignment>
class offset_ptr;

//////////////////////////////////////////////////////////////////////////////
//                    Memory allocation algorithms
//////////////////////////////////////////////////////////////////////////////

//Single segment memory allocation algorithms
template<class MutexFamily, class VoidMutex = offset_ptr<void> >
class simple_seq_fit;

template<class MutexFamily, class VoidMutex = offset_ptr<void>, std::size_t MemAlignment = 0>
class rbtree_best_fit;

//////////////////////////////////////////////////////////////////////////////
//                         Index Types
//////////////////////////////////////////////////////////////////////////////

template<class IndexConfig> class flat_map_index;
template<class IndexConfig> class iset_index;
template<class IndexConfig> class iunordered_set_index;
template<class IndexConfig> class map_index;
template<class IndexConfig> class null_index;
template<class IndexConfig> class unordered_map_index;

//////////////////////////////////////////////////////////////////////////////
//                         Segment manager
//////////////////////////////////////////////////////////////////////////////

template <class CharType
         ,class MemoryAlgorithm
         ,template<class IndexConfig> class IndexType>
class segment_manager;

//////////////////////////////////////////////////////////////////////////////
//                  External buffer managed memory classes
//////////////////////////////////////////////////////////////////////////////

template <class CharType
         ,class MemoryAlgorithm
         ,template<class IndexConfig> class IndexType>
class basic_managed_external_buffer;

typedef basic_managed_external_buffer
   <char
   ,rbtree_best_fit<null_mutex_family>
   ,iset_index>
managed_external_buffer;

typedef basic_managed_external_buffer
   <wchar_t
   ,rbtree_best_fit<null_mutex_family>
   ,iset_index>
wmanaged_external_buffer;

//////////////////////////////////////////////////////////////////////////////
//                      managed memory classes
//////////////////////////////////////////////////////////////////////////////

template <class CharType
         ,class MemoryAlgorithm
         ,template<class IndexConfig> class IndexType>
class basic_managed_shared_memory;

typedef basic_managed_shared_memory
   <char
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
managed_shared_memory;

typedef basic_managed_shared_memory
   <wchar_t
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
wmanaged_shared_memory;


//////////////////////////////////////////////////////////////////////////////
//                      Windows shared memory managed memory classes
//////////////////////////////////////////////////////////////////////////////

#if defined (BOOST_INTERPROCESS_WINDOWS)

template <class CharType
         ,class MemoryAlgorithm
         ,template<class IndexConfig> class IndexType>
class basic_managed_windows_shared_memory;

typedef basic_managed_windows_shared_memory
   <char
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
managed_windows_shared_memory;

typedef basic_managed_windows_shared_memory
   <wchar_t
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
wmanaged_windows_shared_memory;

#endif   //#if defined (BOOST_INTERPROCESS_WINDOWS)

#if defined(BOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS)

template <class CharType
         ,class MemoryAlgorithm
         ,template<class IndexConfig> class IndexType>
class basic_managed_xsi_shared_memory;

typedef basic_managed_xsi_shared_memory
   <char
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
managed_xsi_shared_memory;

typedef basic_managed_xsi_shared_memory
   <wchar_t
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
wmanaged_xsi_shared_memory;

#endif //#if defined(BOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS)

//////////////////////////////////////////////////////////////////////////////
//                      Fixed address shared memory
//////////////////////////////////////////////////////////////////////////////

typedef basic_managed_shared_memory
   <char
   ,rbtree_best_fit<mutex_family, void*>
   ,iset_index>
fixed_managed_shared_memory;

typedef basic_managed_shared_memory
   <wchar_t
   ,rbtree_best_fit<mutex_family, void*>
   ,iset_index>
wfixed_managed_shared_memory;

//////////////////////////////////////////////////////////////////////////////
//                      Heap memory managed memory classes
//////////////////////////////////////////////////////////////////////////////

template
   <class CharType
   ,class MemoryAlgorithm
   ,template<class IndexConfig> class IndexType>
class basic_managed_heap_memory;

typedef basic_managed_heap_memory
   <char
   ,rbtree_best_fit<null_mutex_family>
   ,iset_index>
managed_heap_memory;

typedef basic_managed_heap_memory
   <wchar_t
   ,rbtree_best_fit<null_mutex_family>
   ,iset_index>
wmanaged_heap_memory;

//////////////////////////////////////////////////////////////////////////////
//                         Mapped file managed memory classes
//////////////////////////////////////////////////////////////////////////////

template
   <class CharType
   ,class MemoryAlgorithm
   ,template<class IndexConfig> class IndexType>
class basic_managed_mapped_file;

typedef basic_managed_mapped_file
   <char
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
managed_mapped_file;

typedef basic_managed_mapped_file
   <wchar_t
   ,rbtree_best_fit<mutex_family>
   ,iset_index>
wmanaged_mapped_file;

//////////////////////////////////////////////////////////////////////////////
//                            Exceptions
//////////////////////////////////////////////////////////////////////////////

class interprocess_exception;
class lock_exception;
class bad_alloc;

//////////////////////////////////////////////////////////////////////////////
//                            Bufferstream
//////////////////////////////////////////////////////////////////////////////

//bufferstream
template <class CharT
         ,class CharTraits = std::char_traits<CharT> >
class basic_bufferbuf;

template <class CharT
         ,class CharTraits = std::char_traits<CharT> >
class basic_ibufferstream;

template <class CharT
         ,class CharTraits = std::char_traits<CharT> >
class basic_obufferstream;

template <class CharT
         ,class CharTraits = std::char_traits<CharT> >
class basic_bufferstream;

//////////////////////////////////////////////////////////////////////////////
//                            Vectorstream
//////////////////////////////////////////////////////////////////////////////

template <class CharVector
         ,class CharTraits = std::char_traits<typename CharVector::value_type> >
class basic_vectorbuf;

template <class CharVector
         ,class CharTraits = std::char_traits<typename CharVector::value_type> >
class basic_ivectorstream;

template <class CharVector
         ,class CharTraits = std::char_traits<typename CharVector::value_type> >
class basic_ovectorstream;

template <class CharVector
         ,class CharTraits = std::char_traits<typename CharVector::value_type> >
class basic_vectorstream;

//////////////////////////////////////////////////////////////////////////////
//                             Smart pointers
//////////////////////////////////////////////////////////////////////////////

template<class T, class Deleter>
class scoped_ptr;

template<class T, class VoidPointer>
class intrusive_ptr;

template<class T, class VoidAllocator, class Deleter>
class shared_ptr;

template<class T, class VoidAllocator, class Deleter>
class weak_ptr;

//////////////////////////////////////////////////////////////////////////////
//                                  IPC
//////////////////////////////////////////////////////////////////////////////

template<class VoidPointer>
class message_queue_t;

typedef message_queue_t<offset_ptr<void> > message_queue;

}}  //namespace lslboost { namespace interprocess {

#endif   //#if !defined(BOOST_INTERPROCESS_DOXYGEN_INVOKED)

#include <lslboost/interprocess/detail/config_end.hpp>

#endif //#ifndef BOOST_INTERPROCESS_FWD_HPP
