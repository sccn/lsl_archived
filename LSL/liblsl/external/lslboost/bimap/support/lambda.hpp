// Boost.Bimap
//
// Copyright (c) 2006-2007 Matias Capeletto
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

/// \file support/lambda.hpp
/// \brief Placeholders definition to help in bimap modify function

#ifndef BOOST_BIMAP_SUPPORT_LAMBDA_HPP
#define BOOST_BIMAP_SUPPORT_LAMBDA_HPP

#if defined(_MSC_VER) && (_MSC_VER>=1200)
#pragma once
#endif

#include <lslboost/config.hpp>

#include <lslboost/lambda/lambda.hpp>

namespace lslboost {
namespace bimaps {

namespace {

/*
lslboost::lambda::placeholder1_type & _first  = lslboost::lambda::_1;
lslboost::lambda::placeholder2_type & _second = lslboost::lambda::_2;

lslboost::lambda::placeholder1_type & _left   = lslboost::lambda::_1;
lslboost::lambda::placeholder2_type & _right  = lslboost::lambda::_2;
*/

lslboost::lambda::placeholder1_type & _key  = lslboost::lambda::_1;
lslboost::lambda::placeholder1_type & _data = lslboost::lambda::_1;

}

} // namespace bimaps
} // namespace lslboost


#endif // BOOST_BIMAP_SUPPORT_LAMBDA_HPP

