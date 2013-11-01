//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2007-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_NODE_TOOLS_HPP
#define BOOST_INTERPROCESS_DETAIL_NODE_TOOLS_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>

#include <lslboost/intrusive/slist.hpp>

namespace lslboost {
namespace interprocess {
namespace ipcdetail {


template<class VoidPointer>
struct node_slist
{
   //This hook will be used to chain the individual nodes
    typedef typename bi::make_slist_base_hook
      <bi::void_pointer<VoidPointer>, bi::link_mode<bi::normal_link> >::type slist_hook_t;

   //A node object will hold node_t when it's not allocated
   struct node_t
      :  public slist_hook_t
   {};

   typedef typename bi::make_slist
      <node_t, bi::linear<true>, bi::base_hook<slist_hook_t> >::type node_slist_t;
};

}  //namespace ipcdetail {
}  //namespace interprocess {
}  //namespace lslboost {

#include <lslboost/interprocess/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTERPROCESS_DETAIL_NODE_TOOLS_HPP
