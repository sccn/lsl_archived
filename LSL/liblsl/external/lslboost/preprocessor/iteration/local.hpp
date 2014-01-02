# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.lslboost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.lslboost.org for most recent version. */
#
# ifndef BOOST_PREPROCESSOR_ITERATION_LOCAL_HPP
# define BOOST_PREPROCESSOR_ITERATION_LOCAL_HPP
#
# include <lslboost/preprocessor/config/config.hpp>
# include <lslboost/preprocessor/slot/slot.hpp>
# include <lslboost/preprocessor/tuple/elem.hpp>
#
# /* BOOST_PP_LOCAL_ITERATE */
#
# define BOOST_PP_LOCAL_ITERATE() <lslboost/preprocessor/iteration/detail/local.hpp>
#
# define BOOST_PP_LOCAL_C(n) (BOOST_PP_LOCAL_S) <= n && (BOOST_PP_LOCAL_F) >= n
# define BOOST_PP_LOCAL_R(n) (BOOST_PP_LOCAL_F) <= n && (BOOST_PP_LOCAL_S) >= n
#
# endif
