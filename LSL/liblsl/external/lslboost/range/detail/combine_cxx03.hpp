// Boost.Range library
//
//  Copyright Neil Groves 2014. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_DETAIL_COMBINE_CXX03_HPP
#define BOOST_RANGE_DETAIL_COMBINE_CXX03_HPP

#ifndef BOOST_RANGE_MIN_COMBINE_ARGS
#define BOOST_RANGE_MIN_COMBINE_ARGS 2
#endif

#ifndef BOOST_RANGE_MAX_COMBINE_ARGS
#define BOOST_RANGE_MAX_COMBINE_ARGS 5
#endif

#include <lslboost/config.hpp>
#include <lslboost/iterator/zip_iterator.hpp>
#include <lslboost/preprocessor/arithmetic/dec.hpp>
#include <lslboost/preprocessor/arithmetic/div.hpp>
#include <lslboost/preprocessor/arithmetic/mul.hpp>
#include <lslboost/preprocessor/control.hpp>
#include <lslboost/preprocessor/control/while.hpp>
#include <lslboost/preprocessor/facilities/empty.hpp>
#include <lslboost/preprocessor/facilities/identity.hpp>
#include <lslboost/preprocessor/iteration/local.hpp>
#include <lslboost/preprocessor/punctuation/comma.hpp>
#include <lslboost/preprocessor/repetition.hpp>
#include <lslboost/preprocessor/tuple/elem.hpp>
#include <lslboost/range/iterator_range_core.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/mpl/transform.hpp>
#include <lslboost/utility/result_of.hpp>

#include <vector>
#include <list>

namespace lslboost
{
    namespace range_detail
    {

template<typename F, typename T, int SIZE>
struct combined_result_impl;

template<typename F, typename T>
struct combined_result
    : combined_result_impl<F, T, tuples::length<T>::value>
{
};

#define BOOST_RANGE_combined_element(z, n, data) \
    typename tuples::element<n, T>::type

#define BOOST_RANGE_combined_result(z, n, data) \
    template<typename F, typename T> \
    struct combined_result_impl <F,T,n> \
        : result_of<F(BOOST_PP_ENUM(n, BOOST_RANGE_combined_element, ~))> \
    { \
    };

#define BOOST_PP_LOCAL_MACRO(n) BOOST_RANGE_combined_result(~,n,~)

#define BOOST_PP_LOCAL_LIMITS (BOOST_RANGE_MIN_COMBINE_ARGS, \
                               BOOST_RANGE_MAX_COMBINE_ARGS)
#include BOOST_PP_LOCAL_ITERATE()

#define BOOST_RANGE_combined_get(z, n, data) get<n>(tuple)

#define BOOST_RANGE_combined_unpack(z, n, data) \
    template<typename F, typename T> inline \
    typename combined_result<F,T>::type \
    unpack_(mpl::int_<n>, F f, const T& tuple) \
    { \
        return f(BOOST_PP_ENUM(n, BOOST_RANGE_combined_get, ~)); \
    }

#define BOOST_PP_LOCAL_MACRO(n) BOOST_RANGE_combined_unpack(~,n,~)
#define BOOST_PP_LOCAL_LIMITS (BOOST_RANGE_MIN_COMBINE_ARGS, \
                               BOOST_RANGE_MAX_COMBINE_ARGS)
#include BOOST_PP_LOCAL_ITERATE()

} // namespace range_detail

namespace range
{

#define BOOST_RANGE_combined_seq(z, n, data) lslboost::data(BOOST_PP_CAT(r,n))

#ifdef BOOST_NO_CXX11_RVALUE_REFERENCES

#include <lslboost/range/detail/combine_no_rvalue.hpp>

#else // by using rvalue references we avoid requiring 2^n overloads.

#include <lslboost/range/detail/combine_rvalue.hpp>

#endif

#define BOOST_PP_LOCAL_MACRO(n) BOOST_RANGE_combine(~,n,~)
#define BOOST_PP_LOCAL_LIMITS (BOOST_RANGE_MIN_COMBINE_ARGS, \
                               BOOST_RANGE_MAX_COMBINE_ARGS)
#include BOOST_PP_LOCAL_ITERATE()

    } // namespace range

    using lslboost::range::combine;

} // namespace lslboost

#endif // include guard

#undef BOOST_RANGE_combined_element
#undef BOOST_RANGE_combined_result
#undef BOOST_RANGE_combined_get
#undef BOOST_RANGE_combined_unpack
#undef BOOST_RANGE_combined_seq
#undef BOOST_RANGE_combined_exp_pred
#undef BOOST_RANGE_combined_exp_op
#undef BOOST_RANGE_combined_exp
#undef BOOST_RANGE_combined_bitset_pred
#undef BOOST_RANGE_combined_bitset_op
#undef BOOST_RANGE_combined_bitset
#undef BOOST_RANGE_combined_range_iterator
#undef BOOST_RANGE_combined_args
#undef BOOST_RANGE_combine_impl
#undef BOOST_RANGE_combine
