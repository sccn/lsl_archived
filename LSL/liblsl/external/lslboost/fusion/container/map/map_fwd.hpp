/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_MAP_FORWARD_MAIN_07212005_1105)
#define FUSION_MAP_FORWARD_MAIN_07212005_1105

#include <lslboost/config.hpp>

///////////////////////////////////////////////////////////////////////////////
// With no decltype and variadics, we will use the C++03 version
///////////////////////////////////////////////////////////////////////////////
#if (defined(BOOST_NO_CXX11_DECLTYPE)             \
  || defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)   \
  || defined(BOOST_NO_CXX11_RVALUE_REFERENCES))
# include <lslboost/fusion/container/map/detail/cpp03/map_fwd.hpp>
#else
# if !defined(BOOST_FUSION_HAS_VARIADIC_MAP)
#   define BOOST_FUSION_HAS_VARIADIC_MAP
#endif

#include <lslboost/fusion/container/map/detail/map_impl.hpp>

///////////////////////////////////////////////////////////////////////////////
// C++11 interface
///////////////////////////////////////////////////////////////////////////////
namespace lslboost { namespace fusion
{
    template <typename ...T>
    struct map;
}}

#endif
#endif
