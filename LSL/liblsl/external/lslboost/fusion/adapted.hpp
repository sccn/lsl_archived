/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2005-2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_ADAPTED_30122005_1420)
#define BOOST_FUSION_ADAPTED_30122005_1420

#include <lslboost/fusion/adapted/adt.hpp>
#include <lslboost/fusion/adapted/array.hpp>
#include <lslboost/fusion/adapted/lslboost_array.hpp>
#include <lslboost/fusion/adapted/lslboost_tuple.hpp>
#include <lslboost/fusion/adapted/mpl.hpp>
#include <lslboost/fusion/adapted/std_pair.hpp>
#include <lslboost/fusion/adapted/struct.hpp>

// The std_tuple_iterator adaptor only supports implementations
// using variadic templates
#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <lslboost/fusion/adapted/std_tuple.hpp>
#endif

#endif
