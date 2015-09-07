
#ifndef BOOST_MPL_AUX_TEST_HPP_INCLUDED
#define BOOST_MPL_AUX_TEST_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2002-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <lslboost/mpl/aux_/test/test_case.hpp>
#include <lslboost/mpl/aux_/test/data.hpp>
#include <lslboost/mpl/aux_/test/assert.hpp>
#include <lslboost/detail/lightweight_test.hpp>

#include <lslboost/type_traits/is_same.hpp>

int main()
{
    return lslboost::report_errors();
}

using namespace lslboost;
using namespace mpl;

#endif // BOOST_MPL_AUX_TEST_HPP_INCLUDED
