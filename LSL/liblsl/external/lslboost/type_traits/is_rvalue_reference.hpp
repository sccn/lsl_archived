
//  (C) John Maddock 2010. 
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//
//  See http://www.lslboost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_IS_RVALUE_REFERENCE_HPP_INCLUDED
#define BOOST_TT_IS_RVALUE_REFERENCE_HPP_INCLUDED

#include <lslboost/type_traits/config.hpp>

// should be the last #include
#include <lslboost/type_traits/detail/bool_trait_def.hpp>

namespace lslboost {

BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_rvalue_reference,T,false)
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
BOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC1_1(typename T,is_rvalue_reference,T&&,true)
#endif

} // namespace lslboost

#include <lslboost/type_traits/detail/bool_trait_undef.hpp>

#endif // BOOST_TT_IS_REFERENCE_HPP_INCLUDED

