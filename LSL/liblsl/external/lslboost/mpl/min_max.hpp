
#ifndef BOOST_MPL_MIN_MAX_HPP_INCLUDED
#define BOOST_MPL_MIN_MAX_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2008
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <lslboost/mpl/less.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/mpl/aux_/na_spec.hpp>

namespace lslboost { namespace mpl {

template<
      typename BOOST_MPL_AUX_NA_PARAM(N1)
    , typename BOOST_MPL_AUX_NA_PARAM(N2)
    >
struct min
    : if_< less<N1,N2>,N1,N2 >
{
};

template<
      typename BOOST_MPL_AUX_NA_PARAM(N1)
    , typename BOOST_MPL_AUX_NA_PARAM(N2)
    >
struct max
    : if_< less<N1,N2>,N2,N1 >
{
};

BOOST_MPL_AUX_NA_SPEC(2, min)
BOOST_MPL_AUX_NA_SPEC(2, max)

}}

#endif // BOOST_MPL_MIN_MAX_HPP_INCLUDED
