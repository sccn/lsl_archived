
// Copyright (C) 2009-2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
// Home at http://www.lslboost.org/libs/local_function

#ifndef BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_VALIDATE_HPP_
#define BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_VALIDATE_HPP_

#include <lslboost/local_function/aux_/preprocessor/traits/decl_/validate_/this_count.hpp>
#include <lslboost/local_function/aux_/preprocessor/traits/decl_/validate_/return_count.hpp>

// PUBLIC //

// Validate params after they have been parsed.
#define BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_VALIDATE(decl_traits) \
    BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_VALIDATE_THIS_COUNT( \
    BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_VALIDATE_RETURN_COUNT( \
        decl_traits \
    ))

#endif // #include guard

