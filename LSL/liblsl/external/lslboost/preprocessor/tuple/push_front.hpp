# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2013.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.lslboost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.lslboost.org for most recent version. */
#
# ifndef BOOST_PREPROCESSOR_TUPLE_PUSH_FRONT_HPP
# define BOOST_PREPROCESSOR_TUPLE_PUSH_FRONT_HPP
#
# include <lslboost/preprocessor/config/config.hpp>
#
# if BOOST_PP_VARIADICS
#
# include <lslboost/preprocessor/array/push_front.hpp>
# include <lslboost/preprocessor/array/to_tuple.hpp>
# include <lslboost/preprocessor/tuple/to_array.hpp>
#
#
# /* BOOST_PP_TUPLE_PUSH_FRONT */
#
# define BOOST_PP_TUPLE_PUSH_FRONT(tuple, elem) \
	BOOST_PP_ARRAY_TO_TUPLE(BOOST_PP_ARRAY_PUSH_FRONT(BOOST_PP_TUPLE_TO_ARRAY(tuple), elem)) \
/**/
#
# endif // BOOST_PP_VARIADICS
#
# endif // BOOST_PREPROCESSOR_TUPLE_PUSH_FRONT_HPP
