/*=============================================================================
    Copyright (c) 2012 Kohei Takahashi

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_CLEAR_10172012_0100)
#define BOOST_FUSION_CLEAR_10172012_0100

#include <lslboost/mpl/identity.hpp>
#include <lslboost/fusion/adapted/lslboost_tuple/tag_of.hpp>

namespace lslboost { namespace fusion { namespace detail {

  template <typename Tag>
  struct clear;

  template <>
  struct clear<lslboost_tuple_tag> : mpl::identity<lslboost::tuple<> > {};

}}}

#endif
