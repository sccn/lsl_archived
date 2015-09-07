// Boost.TypeErasure library
//
// Copyright 2012 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_DETAIL_CHECK_MAP_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_CHECK_MAP_HPP_INCLUDED

#include <lslboost/mpl/not.hpp>
#include <lslboost/mpl/or.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/set.hpp>
#include <lslboost/mpl/has_key.hpp>
#include <lslboost/mpl/find_if.hpp>
#include <lslboost/mpl/end.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <lslboost/type_erasure/detail/get_placeholders.hpp>
#include <lslboost/type_erasure/deduced.hpp>
#include <lslboost/type_erasure/static_binding.hpp>

namespace lslboost {
namespace type_erasure {
namespace detail {

template<class T>
struct is_deduced : lslboost::mpl::false_ {};
template<class T>
struct is_deduced< ::lslboost::type_erasure::deduced<T> > : lslboost::mpl::true_ {};

// returns true if Map has a key for every non-deduced placeholder in Concept
template<class Concept, class Map>
struct check_map {
    typedef typename normalize_concept<Concept>::basic basic_components;
    
    // Every non-deduced placeholder referenced in this
    // map is indirectly deduced.
    typedef typename ::lslboost::type_erasure::detail::get_placeholder_normalization_map<
        Concept>::type placeholder_subs;
    typedef typename ::lslboost::mpl::fold<
        placeholder_subs,
        ::lslboost::mpl::set0<>,
        ::lslboost::mpl::insert<
            ::lslboost::mpl::_1,
            ::lslboost::mpl::second< ::lslboost::mpl::_2>
        >
    >::type indirect_deduced_placeholders;

    typedef typename ::lslboost::mpl::fold<
        basic_components,
        ::lslboost::mpl::set0<>,
        ::lslboost::type_erasure::detail::get_placeholders<
            ::lslboost::mpl::_2,
            ::lslboost::mpl::_1
        >
    >::type placeholders;
    typedef typename ::lslboost::is_same<
        typename ::lslboost::mpl::find_if<
            placeholders,
            ::lslboost::mpl::not_<
                ::lslboost::mpl::or_<
                    ::lslboost::type_erasure::detail::is_deduced< ::lslboost::mpl::_1>,
                    ::lslboost::mpl::has_key<Map, ::lslboost::mpl::_1>,
                    ::lslboost::mpl::has_key<indirect_deduced_placeholders, ::lslboost::mpl::_1>
                >
            >
        >::type,
        typename ::lslboost::mpl::end<placeholders>::type
    >::type type;
};

template<class Concept, class Map>
struct check_map<Concept, ::lslboost::type_erasure::static_binding<Map> > :
    check_map<Concept, Map>
{};

}
}
}

#endif
