
// Copyright (C) 2009-2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
// Home at http://www.lslboost.org/libs/local_function

#ifndef BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_SIGN_VALIDATE_HPP_
#define BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_SIGN_VALIDATE_HPP_

#include <lslboost/local_function/aux_/preprocessor/traits/decl_sign_/validate_/defaults.hpp>
#include <lslboost/local_function/aux_/preprocessor/traits/decl_sign_/validate_/this.hpp>
#include <lslboost/preprocessor/tuple/eat.hpp>

// PRIVATE //

#define BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_SIGN_VALIDATE_1_(sign, error) \
    BOOST_PP_IIF(BOOST_PP_IS_EMPTY(error(/* expand empty */)), \
        BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_SIGN_VALIDATE_THIS \
    , \
        error BOOST_PP_TUPLE_EAT(1) \
    )(sign)

// PUBLIC //

// Validate params before starting to parse it.
#define BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_SIGN_VALIDATE(sign) \
    BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_SIGN_VALIDATE_1_(sign, \
        BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_SIGN_VALIDATE_DEFAULTS(sign))

#endif // #include guard

