// Boost.TypeErasure library
//
// Copyright 2012 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#if !defined(BOOST_PP_IS_ITERATING)

#ifndef BOOST_TYPE_ERASURE_DETAIL_INSTANTIATE_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_INSTANTIATE_HPP_INCLUDED

#include <lslboost/mpl/transform.hpp>
#include <lslboost/mpl/size.hpp>
#include <lslboost/mpl/at.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/preprocessor/iteration/iterate.hpp>
#include <lslboost/preprocessor/repetition/repeat.hpp>
#include <lslboost/preprocessor/repetition/enum.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/type_erasure/detail/normalize.hpp>
#include <lslboost/type_erasure/detail/rebind_placeholders.hpp>

namespace lslboost {
namespace type_erasure {
namespace detail {

template<int N>
struct make_instantiate_concept_impl;

template<class Concept>
struct make_instantiate_concept {
    typedef typename ::lslboost::type_erasure::detail::normalize_concept<
        Concept>::type normalized;
    typedef typename ::lslboost::type_erasure::detail::make_instantiate_concept_impl<
        (::lslboost::mpl::size<normalized>::value)
    >::type type;
};

#define BOOST_TYPE_ERASURE_INSTANTIATE(Concept, Map)            \
    (::lslboost::type_erasure::detail::make_instantiate_concept<   \
        Concept                                                 \
    >::type::apply((Concept*)0, (Map*)0))

#define BOOST_TYPE_ERASURE_INSTANTIATE1(Concept, P0, T0)        \
    (::lslboost::type_erasure::detail::make_instantiate_concept<   \
        Concept                                                 \
    >::type::apply(                                             \
        (Concept*)0,                                            \
        (::lslboost::mpl::map1< ::lslboost::mpl::pair<P0, T0> >*)0))

#define BOOST_PP_FILENAME_1 <lslboost/type_erasure/detail/instantiate.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_TYPE_ERASURE_MAX_FUNCTIONS)
#include BOOST_PP_ITERATE()

}
}
}

#endif

#else

#define N BOOST_PP_ITERATION()

#define BOOST_TYPE_ERASURE_INSTANTIATE_IMPL(z, n, data)\
    (void)&::lslboost::mpl::at_c<data, n>::type::apply;

struct BOOST_PP_CAT(instantiate_concept, N) {
    template<class Concept, class Map>
    static void apply(Concept *, Map *) {
        typedef typename ::lslboost::type_erasure::detail::normalize_concept<
            Concept>::type normalized;
        typedef typename ::lslboost::type_erasure::detail::get_placeholder_normalization_map<
            Concept
        >::type placeholder_subs;

        typedef typename ::lslboost::mpl::transform<
            normalized,
            ::lslboost::type_erasure::detail::rebind_placeholders<
                ::lslboost::mpl::_1,
                typename ::lslboost::type_erasure::detail::add_deductions<
                    Map,
                    placeholder_subs
                >::type
            >
        >::type concept_sequence;
        BOOST_PP_REPEAT(N, BOOST_TYPE_ERASURE_INSTANTIATE_IMPL, concept_sequence)
    }
};

template<>
struct make_instantiate_concept_impl<N>
{
    typedef ::lslboost::type_erasure::detail::BOOST_PP_CAT(instantiate_concept, N) type;
};

#undef BOOST_TYPE_ERASURE_INSTANTIATE_IMPL

#undef N

#endif
