
// Copyright (C) 2009-2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
// Home at http://www.lslboost.org/libs/local_function

#ifndef BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_ADD_HPP_
#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_ADD_HPP_

#include <lslboost/preprocessor/control/expr_iif.hpp>
#include <lslboost/preprocessor/logical/compl.hpp>

// `is_front_macro(tokens)` is 1 if `tokens` start w/ `keyword` to add, else 0.
#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_ADD_FRONT( \
        tokens, is_front_macro, keyword) \
    BOOST_PP_EXPR_IIF(BOOST_PP_COMPL(is_front_macro(tokens)), keyword) tokens

// `is_back_macro(tokens)` is 1 if `tokens` end with `keyword` to add, else 0.
#define BOOST_LOCAL_FUNCTION_DETAIL_PP_KEYWORD_FACILITY_ADD_BACK( \
        tokens, is_back_macro, keyword) \
    tokens BOOST_PP_EXPR_IIF(BOOST_PP_COMPL(is_back_macro(tokens)), keyword)

#endif // #include guard

