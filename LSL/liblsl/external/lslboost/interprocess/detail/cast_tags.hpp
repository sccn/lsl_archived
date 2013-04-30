//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_CAST_TAGS_HPP
#define BOOST_INTERPROCESS_CAST_TAGS_HPP

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>

namespace lslboost { namespace interprocess { namespace ipcdetail {

struct static_cast_tag {};
struct const_cast_tag {};
struct dynamic_cast_tag {};
struct reinterpret_cast_tag {};

}}}  //namespace lslboost { namespace interprocess { namespace ipcdetail {

#include <lslboost/interprocess/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTERPROCESS_CAST_TAGS_HPP

