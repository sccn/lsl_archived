# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2014.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.lslboost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.lslboost.org for most recent version. */
#
#ifndef BOOST_PREPROCESSOR_REMOVE_PARENS_HPP
#define BOOST_PREPROCESSOR_REMOVE_PARENS_HPP

#include <lslboost/preprocessor/config/config.hpp>

#if BOOST_PP_VARIADICS

#include <lslboost/preprocessor/control/iif.hpp>
#include <lslboost/preprocessor/facilities/identity.hpp>
#include <lslboost/preprocessor/punctuation/is_begin_parens.hpp>
#include <lslboost/preprocessor/tuple/enum.hpp>

#define BOOST_PP_REMOVE_PARENS(param) \
    BOOST_PP_IIF \
      ( \
      BOOST_PP_IS_BEGIN_PARENS(param), \
      BOOST_PP_REMOVE_PARENS_DO, \
      BOOST_PP_IDENTITY \
      ) \
    (param)() \
/**/

#define BOOST_PP_REMOVE_PARENS_DO(param) \
  BOOST_PP_IDENTITY(BOOST_PP_TUPLE_ENUM(param)) \
/**/

#endif /* BOOST_PP_VARIADICS */
#endif /* BOOST_PREPROCESSOR_REMOVE_PARENS_HPP */
