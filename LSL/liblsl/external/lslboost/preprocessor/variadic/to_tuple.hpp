# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.lslboost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.lslboost.org for most recent version. */
#
# ifndef BOOST_PREPROCESSOR_VARIADIC_TO_TUPLE_HPP
# define BOOST_PREPROCESSOR_VARIADIC_TO_TUPLE_HPP
#
# include <lslboost/preprocessor/config/config.hpp>
#
# /* BOOST_PP_VARIADIC_TO_TUPLE */
#
# if BOOST_PP_VARIADICS
#    define BOOST_PP_VARIADIC_TO_TUPLE(...) (__VA_ARGS__)
# endif
#
# endif
