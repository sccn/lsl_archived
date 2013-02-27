
#ifndef BOOST_MPL_AUX_HAS_REBIND_HPP_INCLUDED
#define BOOST_MPL_AUX_HAS_REBIND_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/mpl for documentation.

// $Id: has_rebind.hpp 49267 2008-10-11 06:19:02Z agurtovoy $
// $Date: 2008-10-11 02:19:02 -0400 (Sat, 11 Oct 2008) $
// $Revision: 49267 $

#include <lslboost/mpl/aux_/config/msvc.hpp>
#include <lslboost/mpl/aux_/config/intel.hpp>
#include <lslboost/mpl/aux_/config/workaround.hpp>

#if BOOST_WORKAROUND(__EDG_VERSION__, <= 244) && !defined(BOOST_INTEL_CXX_VERSION)
#   include <lslboost/mpl/has_xxx.hpp>
#elif BOOST_WORKAROUND(BOOST_MSVC, < 1300)
#   include <lslboost/mpl/has_xxx.hpp>
#   include <lslboost/mpl/if.hpp>
#   include <lslboost/mpl/bool.hpp>
#   include <lslboost/mpl/aux_/msvc_is_class.hpp>
#elif BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x610))
#   include <lslboost/mpl/if.hpp>
#   include <lslboost/mpl/bool.hpp>
#   include <lslboost/mpl/aux_/yes_no.hpp>
#   include <lslboost/mpl/aux_/config/static_constant.hpp>
#   include <lslboost/type_traits/is_class.hpp>
#else
#   include <lslboost/mpl/aux_/type_wrapper.hpp>
#   include <lslboost/mpl/aux_/yes_no.hpp>
#   include <lslboost/mpl/aux_/config/static_constant.hpp>
#endif

namespace lslboost { namespace mpl { namespace aux {

#if BOOST_WORKAROUND(__EDG_VERSION__, <= 244) && !defined(BOOST_INTEL_CXX_VERSION)

BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_rebind, rebind, false)

#elif BOOST_WORKAROUND(BOOST_MSVC, < 1300)

BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_rebind_impl, rebind, false)

template< typename T >
struct has_rebind
    : if_< 
          msvc_is_class<T>
        , has_rebind_impl<T>
        , bool_<false>
        >::type
{
};

#else // the rest

template< typename T > struct has_rebind_tag {};
no_tag operator|(has_rebind_tag<int>, void const volatile*);

#   if !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x610))
template< typename T >
struct has_rebind
{
    static has_rebind_tag<T>* get();
    BOOST_STATIC_CONSTANT(bool, value = 
          sizeof(has_rebind_tag<int>() | get()) == sizeof(yes_tag)
        );
};
#   else // __BORLANDC__
template< typename T >
struct has_rebind_impl
{
    static T* get();
    BOOST_STATIC_CONSTANT(bool, value = 
          sizeof(has_rebind_tag<int>() | get()) == sizeof(yes_tag)
        );
};

template< typename T >
struct has_rebind
    : if_< 
          is_class<T>
        , has_rebind_impl<T>
        , bool_<false>
        >::type
{
};
#   endif // __BORLANDC__

#endif

}}}

#endif // BOOST_MPL_AUX_HAS_REBIND_HPP_INCLUDED