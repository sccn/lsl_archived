// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_DETAIL_ACCESS_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_ACCESS_HPP_INCLUDED

#include <lslboost/type_erasure/detail/storage.hpp>
#include <lslboost/type_erasure/detail/any_base.hpp>

namespace lslboost {
namespace type_erasure {

template<class Concept, class T>
class any;

template<class Concept, class T>
class param;

namespace detail {

struct access
{
    template<class Derived>
    static const typename Derived::table_type&
    table(const ::lslboost::type_erasure::any_base<Derived>& arg)
    {
        return static_cast<const Derived&>(arg).table;
    }
    template<class Concept, class T>
    static const typename any<Concept, T>::table_type&
    table(const ::lslboost::type_erasure::param<Concept, T>& arg)
    {
        return arg._impl.table;
    }
    template<class Derived>
    static ::lslboost::type_erasure::detail::storage&
    data(::lslboost::type_erasure::any_base<Derived>& arg)
    {
        return static_cast<Derived&>(arg).data;
    }
    template<class Concept, class T>
    static const ::lslboost::type_erasure::detail::storage&
    data(::lslboost::type_erasure::any_base< ::lslboost::type_erasure::any<Concept, const T&> >& arg)
    {
        return static_cast< ::lslboost::type_erasure::any<Concept, const T&>&>(arg).data;
    }
    template<class Derived>
    static const ::lslboost::type_erasure::detail::storage&
    data(const ::lslboost::type_erasure::any_base<Derived>& arg)
    {
        return static_cast<const Derived&>(arg).data;
    }
    template<class Concept, class T>
    static ::lslboost::type_erasure::detail::storage&
    data(const ::lslboost::type_erasure::any_base< ::lslboost::type_erasure::any<Concept, T&> >& arg)
    {
        return const_cast< ::lslboost::type_erasure::detail::storage&>(static_cast< const ::lslboost::type_erasure::any<Concept, T&>&>(arg).data);
    }
    template<class Concept, class T>
    static const ::lslboost::type_erasure::detail::storage&
    data(const ::lslboost::type_erasure::any_base< ::lslboost::type_erasure::any<Concept, const T&> >& arg)
    {
        return static_cast<const ::lslboost::type_erasure::any<Concept, const T&>&>(arg).data;
    }
    template<class Concept, class T>
    static ::lslboost::type_erasure::detail::storage&
    data(::lslboost::type_erasure::param<Concept, T>& arg)
    {
        return arg._impl.data;
    }
    template<class Concept, class T>
    static const ::lslboost::type_erasure::detail::storage&
    data(::lslboost::type_erasure::param<Concept, const T&>& arg)
    {
        return arg._impl.data;
    }
    template<class Concept, class T>
    static const ::lslboost::type_erasure::detail::storage&
    data(const ::lslboost::type_erasure::param<Concept, T>& arg)
    {
        return arg._impl.data;
    }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

    template<class Derived>
    static ::lslboost::type_erasure::detail::storage&&
    data(::lslboost::type_erasure::any_base<Derived>&& arg)
    {
        return std::move(static_cast<Derived&>(arg).data);
    }
    template<class Concept, class T>
    static ::lslboost::type_erasure::detail::storage&&
    data(::lslboost::type_erasure::any_base< ::lslboost::type_erasure::any<Concept, T&&> >& arg)
    {
        return std::move(static_cast< ::lslboost::type_erasure::any<Concept, T&&>&>(arg).data);
    }
    template<class Concept, class T>
    static ::lslboost::type_erasure::detail::storage&&
    data(const ::lslboost::type_erasure::any_base< ::lslboost::type_erasure::any<Concept, T&&> >& arg)
    {
        return std::move(const_cast< ::lslboost::type_erasure::detail::storage&>(static_cast< const ::lslboost::type_erasure::any<Concept, T&&>&>(arg).data));
    }

    template<class Concept, class T>
    static ::lslboost::type_erasure::detail::storage&&
    data(::lslboost::type_erasure::param<Concept, T>&& arg)
    {
        return std::move(arg._impl.data);
    }
    template<class Concept, class T>
    static ::lslboost::type_erasure::detail::storage&&
    data(::lslboost::type_erasure::param<Concept, T&&>& arg)
    {
        return std::move(arg._impl.data);
    }
    template<class Concept, class T>
    static ::lslboost::type_erasure::detail::storage&&
    data(const ::lslboost::type_erasure::param<Concept, T&&>& arg)
    {
        return std::move(const_cast< ::lslboost::type_erasure::detail::storage&>(arg._impl.data));
    }

#endif

};

}
}
}

#endif
