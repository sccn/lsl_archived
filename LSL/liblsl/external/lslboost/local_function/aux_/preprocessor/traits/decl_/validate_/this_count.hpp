
// Copyright (C) 2009-2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
// Home at http://www.lslboost.org/libs/local_function

#ifndef BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_VALIDATE_THIS_COUNT_HPP_
#define BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_VALIDATE_THIS_COUNT_HPP_

#include <lslboost/local_function/aux_/preprocessor/traits/decl_/set_error.hpp>
#include <lslboost/local_function/aux_/preprocessor/traits/decl_const_binds.hpp>
#include <lslboost/local_function/aux_/preprocessor/traits/decl_binds.hpp>
#include <lslboost/preprocessor/control/iif.hpp>
#include <lslboost/preprocessor/facilities/empty.hpp>
#include <lslboost/preprocessor/tuple/eat.hpp>
#include <lslboost/preprocessor/comparison/greater.hpp>
#include <lslboost/preprocessor/list/append.hpp>
#include <lslboost/preprocessor/list/size.hpp>

// PUBLIC //

#define BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_VALIDATE_THIS_COUNT( \
        decl_traits) \
    BOOST_PP_IIF(BOOST_PP_GREATER(BOOST_PP_LIST_SIZE(BOOST_PP_LIST_APPEND( \
            BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_CONST_BIND_THIS_TYPES( \
                    decl_traits), \
            BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_BIND_THIS_TYPES( \
                    decl_traits))), \
            1), \
        BOOST_LOCAL_FUNCTION_AUX_PP_DECL_TRAITS_SET_ERROR \
    , /* do nothing (keeping previous error, if any) */ \
        decl_traits BOOST_PP_TUPLE_EAT(2) \
    )(decl_traits, /* trailing `EMPTY` because error might not be present */ \
            ERROR_cannot_bind_object_this_multiple_times BOOST_PP_EMPTY)

#endif // #include guard

