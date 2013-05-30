//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_MAP_INDEX_HPP
#define BOOST_INTERPROCESS_MAP_INDEX_HPP

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>

#include <functional>
#include <utility>
#include <lslboost/interprocess/containers/map.hpp>
#include <lslboost/interprocess/allocators/private_adaptive_pool.hpp>

//!\file
//!Describes index adaptor of lslboost::map container, to use it
//!as name/shared memory index

namespace lslboost {
namespace interprocess {
namespace ipcdetail{

//!Helper class to define typedefs from IndexTraits
template <class MapConfig>
struct map_index_aux
{
   typedef typename MapConfig::key_type            key_type;
   typedef typename MapConfig::mapped_type         mapped_type;
   typedef std::less<key_type>                     key_less;
   typedef std::pair<const key_type, mapped_type>  value_type;

   typedef private_adaptive_pool
            <value_type,
               typename MapConfig::
         segment_manager_base>                     allocator_type;

   typedef lslboost::interprocess::map
      <key_type,  mapped_type,
       key_less, allocator_type>                   index_t;
};

}  //namespace ipcdetail {

//!Index type based in lslboost::interprocess::map. Just derives from lslboost::interprocess::map
//!and defines the interface needed by managed memory segments
template <class MapConfig>
class map_index
   //Derive class from map specialization
   : public ipcdetail::map_index_aux<MapConfig>::index_t
{
   /// @cond
   typedef ipcdetail::map_index_aux<MapConfig>  index_aux;
   typedef typename index_aux::index_t       base_type;
   typedef typename MapConfig::
      segment_manager_base          segment_manager_base;
   /// @endcond

   public:
   //!Constructor. Takes a pointer to the
   //!segment manager. Can throw
   map_index(segment_manager_base *segment_mngr)
      : base_type(typename index_aux::key_less(),
                  segment_mngr){}

   //!This reserves memory to optimize the insertion of n
   //!elements in the index
   void reserve(typename segment_manager_base::size_type)
      {  /*Does nothing, map has not reserve or rehash*/  }

   //!This tries to free previously allocate
   //!unused memory.
   void shrink_to_fit()
   {  base_type::get_stored_allocator().deallocate_free_blocks(); }
};

/// @cond

//!Trait class to detect if an index is a node
//!index. This allows more efficient operations
//!when deallocating named objects.
template<class MapConfig>
struct is_node_index
   <lslboost::interprocess::map_index<MapConfig> >
{
   static const bool value = true;
};
/// @endcond

}}   //namespace lslboost { namespace interprocess {

#include <lslboost/interprocess/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTERPROCESS_MAP_INDEX_HPP
