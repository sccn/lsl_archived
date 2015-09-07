// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_DETAIL_NORMALIZE_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_NORMALIZE_HPP_INCLUDED

#include <lslboost/mpl/assert.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/mpl/is_sequence.hpp>
#include <lslboost/mpl/set.hpp>
#include <lslboost/mpl/map.hpp>
#include <lslboost/mpl/has_key.hpp>
#include <lslboost/mpl/insert.hpp>
#include <lslboost/mpl/vector.hpp>
#include <lslboost/mpl/back_inserter.hpp>
#include <lslboost/mpl/inserter.hpp>
#include <lslboost/mpl/fold.hpp>
#include <lslboost/mpl/transform.hpp>
#include <lslboost/mpl/copy.hpp>
#include <lslboost/mpl/at.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <lslboost/type_erasure/detail/get_placeholders.hpp>
#include <lslboost/type_erasure/detail/rebind_placeholders.hpp>
#include <lslboost/type_erasure/detail/normalize_deduced.hpp>
#include <lslboost/type_erasure/relaxed.hpp>
#include <lslboost/type_erasure/builtin.hpp>

namespace lslboost {
namespace type_erasure {

template<class F>
struct deduced;

template<class T, class U>
struct same_type;

namespace detail {

struct substitution_map_tag {};

// a wrapper around an mpl::map that
// defaults to the identity map.
template<class M>
struct substitution_map
{
    typedef substitution_map_tag tag;
    typedef M map_type;
};

}
}

namespace mpl {

template<>
struct at_impl< ::lslboost::type_erasure::detail::substitution_map_tag>
{
    template<class Seq, class Key>
    struct apply
    {
        typedef typename ::lslboost::mpl::eval_if<
            ::lslboost::mpl::has_key<typename Seq::map_type, Key>,
            ::lslboost::mpl::at<typename Seq::map_type, Key>,
            ::lslboost::mpl::identity<Key>
        >::type type;
    };
};

template<>
struct has_key_impl< ::lslboost::type_erasure::detail::substitution_map_tag>
{
    template<class Seq, class Key>
    struct apply : lslboost::mpl::true_
    {};
};

}

namespace type_erasure {
namespace detail {

// given a partial substitution map from same_type,
// resolves a placeholder as far as possible.
template<class M, class T>
struct resolve_same_type
{
    typedef typename ::lslboost::mpl::eval_if< ::lslboost::mpl::has_key<M, T>,
        ::lslboost::type_erasure::detail::resolve_same_type<
            M,
            typename ::lslboost::mpl::at<M, T>::type
        >,
        ::lslboost::mpl::identity<T>
    >::type type;
};

// Given the arguments to same_type, determines
// which should be the key and which should be
// the value in the substitution map.
template<class T, class U>
struct select_pair
{
    BOOST_MPL_ASSERT((::lslboost::is_same<T, U>));
    typedef void type;
};

template<class T, class U>
struct select_pair<T, ::lslboost::type_erasure::deduced<U> >
{
    typedef ::lslboost::mpl::pair< ::lslboost::type_erasure::deduced<U>, T> type;
};

template<class T, class U>
struct select_pair< ::lslboost::type_erasure::deduced<T>, U>
{
    typedef ::lslboost::mpl::pair< ::lslboost::type_erasure::deduced<T>, U> type;
};

template<class T, class U>
struct select_pair<
    ::lslboost::type_erasure::deduced<T>,
    ::lslboost::type_erasure::deduced<U>
>
{
    typedef ::lslboost::mpl::pair<
        ::lslboost::type_erasure::deduced<T>,
        ::lslboost::type_erasure::deduced<U>
    > type;
};

// M is a map of placeholder substitutions
template<class M, class T>
struct normalize_placeholder
{
    typedef typename ::lslboost::mpl::eval_if< ::lslboost::mpl::has_key<M, T>,
        ::lslboost::type_erasure::detail::normalize_placeholder<
            M,
            typename ::lslboost::mpl::at<M, T>::type
        >,
        ::lslboost::mpl::identity<T>
    >::type type;
};

template<class M, class T>
struct normalize_placeholder<M, ::lslboost::type_erasure::deduced<T> >
{
    typedef typename ::lslboost::mpl::eval_if< ::lslboost::mpl::has_key<M, T>,
        ::lslboost::type_erasure::detail::normalize_placeholder<
            M,
            typename ::lslboost::mpl::at<M, T>::type
        >,
        ::lslboost::type_erasure::detail::normalize_deduced<
            M,
            T
        >
    >::type type;
};

// Takes a mpl::map of placeholder substitutions and
// fully resolves it.  i.e.  a -> b, b -> c, becomes
// a -> c, b -> c.  Also resolves deduced placeholders
// whose arguments are all resolved.
template<class M>
struct create_placeholder_map
{
    typedef typename ::lslboost::mpl::fold<
        M,
        ::lslboost::mpl::map0<>,
        ::lslboost::mpl::insert<
            ::lslboost::mpl::_1,
            ::lslboost::mpl::pair<
                ::lslboost::mpl::first< ::lslboost::mpl::_2>,
                ::lslboost::type_erasure::detail::normalize_placeholder<M, ::lslboost::mpl::second< ::lslboost::mpl::_2> >
            >
        >
    >::type type;
};

template<class Bindings, class P, class Out, class Sub>
struct convert_deduced
{
    typedef typename ::lslboost::type_erasure::detail::rebind_placeholders_in_argument<
        typename P::first,
        Bindings
    >::type result1;
    typedef typename ::lslboost::mpl::at<Sub, result1>::type result;
    typedef typename ::lslboost::mpl::eval_if<
        ::lslboost::mpl::has_key<Out, typename P::second>,
        ::lslboost::mpl::identity<Out>,
        ::lslboost::mpl::insert<Out, ::lslboost::mpl::pair<typename P::second, result> >
    >::type type;
    BOOST_MPL_ASSERT((lslboost::is_same<typename ::lslboost::mpl::at<type, typename P::second>::type, result>));
};

template<class Bindings, class M, class Sub>
struct convert_deductions
{
    typedef typename ::lslboost::mpl::fold<
        M,
        Bindings,
        ::lslboost::type_erasure::detail::convert_deduced<
            Bindings, ::lslboost::mpl::_2, ::lslboost::mpl::_1, Sub
        >
    >::type type;
};

template<class Bindings, class P, class Out>
struct add_deduced
{
    typedef typename ::lslboost::type_erasure::detail::rebind_placeholders_in_argument<
        typename P::first,
        Bindings
    >::type result;
    typedef typename ::lslboost::mpl::eval_if<
        ::lslboost::mpl::has_key<Out, typename P::second>,
        ::lslboost::mpl::identity<Out>,
        ::lslboost::mpl::insert<Out, ::lslboost::mpl::pair<typename P::second, result> >
    >::type type;
    BOOST_MPL_ASSERT((lslboost::is_same<typename ::lslboost::mpl::at<type, typename P::second>::type, result>));
};

template<class Bindings, class M>
struct add_deductions
{
    typedef typename ::lslboost::mpl::fold<
        M,
        Bindings,
        ::lslboost::type_erasure::detail::add_deduced<
            Bindings, ::lslboost::mpl::_2, ::lslboost::mpl::_1
        >
    >::type type;
};

// Fold Op for normalize_concept_impl
template<class Out, class T>
struct insert_concept
{
    typedef ::lslboost::mpl::pair<
        typename ::lslboost::mpl::insert<typename Out::first, T>::type,
        typename Out::second
    > type;
};

template<class Out, class T, class U>
struct insert_concept<Out, ::lslboost::type_erasure::same_type<T, U> >
{
    typedef typename ::lslboost::type_erasure::detail::resolve_same_type<
        typename Out::second,
        T
    >::type t1;
    typedef typename ::lslboost::type_erasure::detail::resolve_same_type<
        typename Out::second,
        U
    >::type t2;
    typedef ::lslboost::mpl::pair<
        typename Out::first,
        typename ::lslboost::mpl::eval_if<
            ::lslboost::is_same<t1, t2>,
            ::lslboost::mpl::identity<typename Out::second>,
            ::lslboost::mpl::insert<
                typename Out::second,
                typename ::lslboost::type_erasure::detail::select_pair<
                    t1,
                    t2
                >::type
            >
        >::type
    > type;
};

// flattens a concept returning an mpl::pair
// - first is an MPL sequence containing the leaf concepts
// - second is an MPL map of the placeholder substitutions
//   used to resolve same_type.
template<class Concept, class Out = ::lslboost::mpl::pair< ::lslboost::mpl::set0<>, ::lslboost::mpl::map0<> > >
struct normalize_concept_impl
{
    typedef typename ::lslboost::mpl::eval_if< ::lslboost::mpl::is_sequence<Concept>,
        ::lslboost::mpl::fold<Concept, Out, normalize_concept_impl< ::lslboost::mpl::_2, ::lslboost::mpl::_1> >,
        ::lslboost::type_erasure::detail::insert_concept<Out, Concept>
    >::type type;
};

struct append_typeinfo
{
    template<class Set, class T>
    struct apply
    {
        typedef typename ::lslboost::mpl::insert<
            Set,
            ::lslboost::type_erasure::typeid_<T>
        >::type type;
    };
};

// Seq should be a flattened MPL sequence of leaf concepts.
// adds typeid_<P> for every placeholder used.
template<class Seq>
struct add_typeinfo
{
    typedef typename ::lslboost::mpl::fold<
        Seq,
        ::lslboost::mpl::set0<>,
        ::lslboost::type_erasure::detail::get_placeholders<
            ::lslboost::mpl::_2,
            ::lslboost::mpl::_1
        >
    >::type placeholders;
    typedef typename ::lslboost::mpl::fold<
        placeholders,
        Seq,
        ::lslboost::type_erasure::detail::append_typeinfo
    >::type type;
};

template<class Concept>
struct get_placeholder_normalization_map
{
    typedef typename ::lslboost::type_erasure::detail::create_placeholder_map<
        typename normalize_concept_impl<Concept>::type::second
    >::type type;
};

// Flattens a Concept to an mpl::vector of primitive
// concepts.  Resolves same_type and deduced placeholders.
template<class Concept>
struct normalize_concept
{
    typedef typename normalize_concept_impl<Concept>::type impl;
    typedef typename ::lslboost::type_erasure::detail::create_placeholder_map<
        typename impl::second
    >::type substitutions;
    typedef typename ::lslboost::mpl::fold<
        typename impl::first,
        ::lslboost::mpl::set0<>,
        ::lslboost::mpl::insert<
            ::lslboost::mpl::_1,
            ::lslboost::type_erasure::detail::rebind_placeholders<
                ::lslboost::mpl::_2,
                ::lslboost::type_erasure::detail::substitution_map<substitutions>
            >
        >
    >::type basic;
    typedef typename ::lslboost::mpl::eval_if<
        ::lslboost::type_erasure::is_relaxed<Concept>,
        ::lslboost::type_erasure::detail::add_typeinfo<basic>,
        ::lslboost::mpl::identity<basic>
    >::type concept_set;
    typedef typename ::lslboost::mpl::copy<
        concept_set,
        ::lslboost::mpl::back_inserter< ::lslboost::mpl::vector0<> >
    >::type type;
};

// Returns an MPL sequence containing all the concepts
// in Concept.  If Concept is considered as a DAG,
// the result will be sorted topologically.
template<
    class Concept,
    class Map = typename ::lslboost::type_erasure::detail::create_placeholder_map<
            typename ::lslboost::type_erasure::detail::normalize_concept_impl<
                Concept
            >::type::second
        >::type,
    class Out = ::lslboost::mpl::set0<>
>
struct collect_concepts
{
    typedef typename ::lslboost::type_erasure::detail::rebind_placeholders<
        Concept,
        ::lslboost::type_erasure::detail::substitution_map<Map>
    >::type transformed;
    typedef typename ::lslboost::mpl::eval_if<
        ::lslboost::is_same<transformed, void>,
        ::lslboost::mpl::identity<Out>,
        ::lslboost::mpl::insert<
            Out,
            transformed
        >
    >::type type1;
    typedef typename ::lslboost::mpl::eval_if< ::lslboost::mpl::is_sequence<Concept>,
        ::lslboost::mpl::fold<Concept, type1, collect_concepts< ::lslboost::mpl::_2, Map, ::lslboost::mpl::_1> >,
        ::lslboost::mpl::identity<type1>
    >::type type;
};

}
}
}

#endif
