# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.lslboost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.lslboost.org for most recent version. */
#
# ifndef BOOST_PREPROCESSOR_SEQ_TO_LIST_HPP
# define BOOST_PREPROCESSOR_SEQ_TO_LIST_HPP
#
# include <lslboost/preprocessor/punctuation/comma.hpp>
# include <lslboost/preprocessor/punctuation/paren.hpp>
# include <lslboost/preprocessor/seq/detail/binary_transform.hpp>
#
# /* BOOST_PP_SEQ_TO_LIST */
#
# define BOOST_PP_SEQ_TO_LIST(seq) BOOST_PP_SEQ_TO_LIST_I(BOOST_PP_SEQ_BINARY_TRANSFORM(seq))
# define BOOST_PP_SEQ_TO_LIST_I(bseq) BOOST_PP_SEQ_TO_LIST_A bseq BOOST_PP_NIL BOOST_PP_SEQ_TO_LIST_B bseq
# define BOOST_PP_SEQ_TO_LIST_A(m, e) m(BOOST_PP_LPAREN() e BOOST_PP_COMMA() BOOST_PP_SEQ_TO_LIST_A_ID)
# define BOOST_PP_SEQ_TO_LIST_A_ID() BOOST_PP_SEQ_TO_LIST_A
# define BOOST_PP_SEQ_TO_LIST_B(m, e) m(BOOST_PP_RPAREN() BOOST_PP_SEQ_TO_LIST_B_ID)
# define BOOST_PP_SEQ_TO_LIST_B_ID() BOOST_PP_SEQ_TO_LIST_B
#
# endif
