// Copyright 2005 Daniel Wallin.
// Copyright 2005 Joel de Guzman.
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// Modeled after range_ex, Copyright 2004 Eric Niebler
///////////////////////////////////////////////////////////////////////////////
//
// is_std_map.hpp
//
/////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER >= 1000
# pragma once
#endif

#ifndef IS_STD_MAP_EN_16_12_2004
#define IS_STD_MAP_EN_16_12_2004

#include <lslboost/mpl/bool.hpp>
#include <lslboost/detail/container_fwd.hpp>

namespace lslboost
{
    template<class T>
    struct is_std_map
        : lslboost::mpl::false_
    {};

    template<
        class Kty
      , class Ty
      , class Pr
      , class Alloc
    >
    struct is_std_map< ::std::map<Kty,Ty,Pr,Alloc> >
        : lslboost::mpl::true_
    {};

    template<class T>
    struct is_std_multimap
        : lslboost::mpl::false_
    {};

    template<
        class Kty
      , class Ty
      , class Pr
      , class Alloc
    >
    struct is_std_multimap< ::std::multimap<Kty,Ty,Pr,Alloc> >
        : lslboost::mpl::true_
    {};
}

#endif
