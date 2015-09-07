///////////////////////////////////////////////////////////////////////////////
//  Copyright 2013 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_MP_UBLAS_HPP
#define BOOST_MP_UBLAS_HPP

namespace lslboost { namespace numeric { namespace ublas {

template<class V>
class sparse_vector_element;

template <class V, class Backend, multiprecision::expression_template_option ExpressionTemplates>
inline bool operator == (const sparse_vector_element<V>& a, const ::lslboost::multiprecision::number<Backend, ExpressionTemplates>& b)
{
   typedef typename sparse_vector_element<V>::const_reference ref_type;
   return static_cast<ref_type>(a) == b;
}

template<class X, class Y>
struct promote_traits;

template <class Backend1, lslboost::multiprecision::expression_template_option ExpressionTemplates1, class Backend2, lslboost::multiprecision::expression_template_option ExpressionTemplates2>
struct promote_traits<lslboost::multiprecision::number<Backend1, ExpressionTemplates1>, lslboost::multiprecision::number<Backend2, ExpressionTemplates2> >
{
   typedef lslboost::multiprecision::number<Backend1, ExpressionTemplates1> number1_t;
   typedef lslboost::multiprecision::number<Backend2, ExpressionTemplates2> number2_t;
   typedef typename mpl::if_c<
      is_convertible<number1_t, number2_t>::value && !is_convertible<number2_t, number1_t>::value,
      number2_t, number1_t
   >::type promote_type;
};

template <class Backend1, lslboost::multiprecision::expression_template_option ExpressionTemplates1, class tag, class Arg1, class Arg2, class Arg3, class Arg4>
struct promote_traits<lslboost::multiprecision::number<Backend1, ExpressionTemplates1>, lslboost::multiprecision::detail::expression<tag, Arg1, Arg2, Arg3, Arg4> >
{
   typedef lslboost::multiprecision::number<Backend1, ExpressionTemplates1> number1_t;
   typedef lslboost::multiprecision::detail::expression<tag, Arg1, Arg2, Arg3, Arg4> expression_type;
   typedef typename expression_type::result_type number2_t;
   typedef typename promote_traits<number1_t, number2_t>::promote_type promote_type;
};

template <class tag, class Arg1, class Arg2, class Arg3, class Arg4, class Backend1, lslboost::multiprecision::expression_template_option ExpressionTemplates1>
struct promote_traits<lslboost::multiprecision::detail::expression<tag, Arg1, Arg2, Arg3, Arg4>, lslboost::multiprecision::number<Backend1, ExpressionTemplates1> >
{
   typedef lslboost::multiprecision::number<Backend1, ExpressionTemplates1> number1_t;
   typedef lslboost::multiprecision::detail::expression<tag, Arg1, Arg2, Arg3, Arg4> expression_type;
   typedef typename expression_type::result_type number2_t;
   typedef typename promote_traits<number1_t, number2_t>::promote_type promote_type;
};

template <class tag, class Arg1, class Arg2, class Arg3, class Arg4, class tagb, class Arg1b, class Arg2b, class Arg3b, class Arg4b>
struct promote_traits<lslboost::multiprecision::detail::expression<tag, Arg1, Arg2, Arg3, Arg4>, lslboost::multiprecision::detail::expression<tagb, Arg1b, Arg2b, Arg3b, Arg4b> >
{
   typedef lslboost::multiprecision::detail::expression<tag, Arg1, Arg2, Arg3, Arg4> expression1_t;
   typedef typename expression1_t::result_type number1_t;
   typedef lslboost::multiprecision::detail::expression<tagb, Arg1b, Arg2b, Arg3b, Arg4b> expression2_t;
   typedef typename expression2_t::result_type number2_t;
};

}}} // namespace

#endif

