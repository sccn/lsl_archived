// Boost.TypeErasure library
//
// Copyright 2012 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_DETAIL_CONST_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_CONST_HPP_INCLUDED

#include <lslboost/mpl/if.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/or.hpp>
#include <lslboost/mpl/and.hpp>
#include <lslboost/mpl/not.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <lslboost/type_traits/is_const.hpp>
#include <lslboost/type_traits/is_reference.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/type_traits/remove_cv.hpp>
#include <lslboost/type_erasure/placeholder_of.hpp>
#include <lslboost/type_erasure/derived.hpp>

namespace lslboost {
namespace type_erasure {
namespace detail {

template<class T>
struct is_non_const_ref : lslboost::mpl::false_ {};
template<class T>
struct is_non_const_ref<T&> : lslboost::mpl::true_ {};
template<class T>
struct is_non_const_ref<const T&> : lslboost::mpl::false_ {};

template<class Placeholder, class Base>
struct should_be_const :
    ::lslboost::mpl::or_<
        ::lslboost::is_const<Placeholder>,
        ::lslboost::type_erasure::detail::is_non_const_ref<
            typename ::lslboost::type_erasure::placeholder_of<Base>::type
        >
    >
{};

template<class Placeholder, class Base>
struct should_be_non_const :
    ::lslboost::mpl::and_<
        ::lslboost::mpl::not_< ::lslboost::is_const<Placeholder> >,
        ::lslboost::mpl::not_<
            ::lslboost::is_reference<
                typename ::lslboost::type_erasure::placeholder_of<Base>::type
            >
        >
    >
{};

template<class Base>
struct non_const_this_param
{
    typedef typename ::lslboost::type_erasure::placeholder_of<Base>::type placeholder;
    typedef typename ::lslboost::type_erasure::derived<Base>::type plain_type;
    typedef typename ::lslboost::mpl::if_<
        ::lslboost::is_same<
            placeholder,
            typename ::lslboost::remove_cv<
                typename ::lslboost::remove_reference<placeholder>::type
            >::type&
        >,
        const plain_type,
        plain_type
    >::type type;
};

template<class T>
struct uncallable {};

template<class Placeholder, class Base>
struct maybe_const_this_param
{
    typedef typename ::lslboost::type_erasure::derived<Base>::type plain_type;
    typedef typename ::lslboost::remove_reference<Placeholder>::type plain_placeholder;
    typedef typename ::lslboost::mpl::if_< ::lslboost::is_reference<Placeholder>,
        typename ::lslboost::mpl::if_<
            ::lslboost::type_erasure::detail::should_be_non_const<plain_placeholder, Base>,
            plain_type&,
            typename ::lslboost::mpl::if_<
                ::lslboost::type_erasure::detail::should_be_const<plain_placeholder, Base>,
                const plain_type&,
                uncallable<plain_type>
            >::type
        >::type,
        plain_type
    >::type type;
};

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template<class Placeholder, class Base>
struct maybe_const_this_param<Placeholder&&, Base>
{
    typedef typename ::lslboost::type_erasure::derived<Base>::type plain_type;
    typedef typename ::lslboost::remove_reference<Placeholder>::type plain_placeholder;
    typedef typename ::lslboost::type_erasure::placeholder_of<plain_type>::type self_placeholder;
    typedef typename ::lslboost::mpl::if_< ::lslboost::is_lvalue_reference<self_placeholder>,
        ::lslboost::type_erasure::detail::uncallable<plain_type>,
        typename ::lslboost::mpl::if_< ::lslboost::is_rvalue_reference<self_placeholder>,
            const plain_type&,
            plain_type&&
        >::type
    >::type type;
};

#endif

}
}
}

#endif
