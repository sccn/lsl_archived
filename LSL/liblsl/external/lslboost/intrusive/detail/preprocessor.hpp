//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/intrusive for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_PREPROCESSOR_HPP
#define BOOST_INTRUSIVE_DETAIL_PREPROCESSOR_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <lslboost/intrusive/detail/config_begin.hpp>
#include <lslboost/intrusive/detail/workaround.hpp>

#include <lslboost/preprocessor/iteration/local.hpp>
#include <lslboost/preprocessor/punctuation/paren_if.hpp>
#include <lslboost/preprocessor/punctuation/comma_if.hpp>
#include <lslboost/preprocessor/control/expr_if.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/preprocessor/repetition/enum.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing.hpp>
#include <lslboost/preprocessor/repetition/enum_shifted_params.hpp>
#include <lslboost/preprocessor/repetition/enum_shifted.hpp>
#include <lslboost/preprocessor/repetition/repeat.hpp>
#include <lslboost/preprocessor/logical/not.hpp>
#include <lslboost/preprocessor/arithmetic/sub.hpp>
#include <lslboost/preprocessor/arithmetic/add.hpp>
#include <lslboost/preprocessor/iteration/iterate.hpp>

#define BOOST_INTRUSIVE_MAX_CONSTRUCTOR_PARAMETERS 10

#define BOOST_INTRUSIVE_PP_IDENTITY(z, n, data) data

#define BOOST_INTRUSIVE_PP_DECLVAL(z, n, data) \
lslboost::move_detail::declval< BOOST_PP_CAT(P, n) >() \
//!

#define BOOST_INTRUSIVE_PP_TEMPLATE_PARAM_VOID_DEFAULT(z, n, data)   \
  BOOST_PP_CAT(class P, n) = void                                      \
//!

#include <lslboost/intrusive/detail/config_end.hpp>

#endif //#ifndef BOOST_INTRUSIVE_DETAIL_PREPROCESSOR_HPP
