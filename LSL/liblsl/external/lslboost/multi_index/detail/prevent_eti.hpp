/* Copyright 2003-2008 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.lslboost.org/LICENSE_1_0.txt)
 *
 * See http://www.lslboost.org/libs/multi_index for library home page.
 */

#ifndef BOOST_MULTI_INDEX_DETAIL_PREVENT_ETI_HPP
#define BOOST_MULTI_INDEX_DETAIL_PREVENT_ETI_HPP

#if defined(_MSC_VER)&&(_MSC_VER>=1200)
#pragma once
#endif

#include <lslboost/config.hpp> /* keep it first to prevent nasty warns in MSVC */
#include <lslboost/detail/workaround.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC,<1300)
#include <lslboost/mpl/if.hpp>
#include <lslboost/mpl/integral_c.hpp>
#include <lslboost/mpl/aux_/msvc_never_true.hpp>
#endif

namespace lslboost{

namespace multi_index{

namespace detail{

#if BOOST_WORKAROUND(BOOST_MSVC,<1300)
/* See
 * http://www.crystalclearsoftware.com/cgi-bin/lslboost_wiki/wiki.pl?Effective_MPL
 * Item 5.6, Beware of the 'early template instantiation' trap.
 */

template<typename Type,typename Construct>
struct prevent_eti
{
  typedef typename mpl::if_<
    mpl::aux::msvc_never_true<Type>,
    mpl::integral_c<int,0>,
    Construct
  >::type type;
};
#else
template<typename Type,typename Construct>
struct prevent_eti
{
  typedef Construct type;
};
#endif

} /* namespace multi_index::detail */

} /* namespace multi_index */

} /* namespace lslboost */

#endif
