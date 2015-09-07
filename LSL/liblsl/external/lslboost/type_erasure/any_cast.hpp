// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_ANY_CAST_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_ANY_CAST_HPP_INCLUDED

#include <stdexcept>
#include <lslboost/throw_exception.hpp>
#include <lslboost/type_traits/add_const.hpp>
#include <lslboost/type_traits/is_pointer.hpp>
#include <lslboost/type_traits/remove_cv.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/type_traits/remove_pointer.hpp>
#include <lslboost/type_traits/is_void.hpp>
#include <lslboost/mpl/assert.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/type_erasure/any.hpp>
#include <lslboost/type_erasure/builtin.hpp>
#include <lslboost/type_erasure/exception.hpp>
#include <lslboost/type_erasure/detail/access.hpp>

namespace lslboost {
namespace type_erasure {

namespace detail {

template<class Concept, class T>
void* get_pointer(::lslboost::type_erasure::any<Concept, T>& arg)
{
    return ::lslboost::type_erasure::detail::access::data(arg).data;
}

template<class Concept, class T>
const void* get_pointer(const ::lslboost::type_erasure::any<Concept, T>& arg)
{
    return ::lslboost::type_erasure::detail::access::data(arg).data;
}

template<class Concept, class T>
void* get_pointer(::lslboost::type_erasure::any<Concept, T&>& arg)
{
    return ::lslboost::type_erasure::detail::access::data(arg).data;
}

template<class Concept, class T>
void* get_pointer(const ::lslboost::type_erasure::any<Concept, T&>& arg)
{
    return ::lslboost::type_erasure::detail::access::data(arg).data;
}

template<class Concept, class T>
const void* get_pointer(::lslboost::type_erasure::any<Concept, const T&>& arg)
{
    return ::lslboost::type_erasure::detail::access::data(arg).data;
}

template<class Concept, class T>
const void* get_pointer(const ::lslboost::type_erasure::any<Concept, const T&>& arg)
{
    return ::lslboost::type_erasure::detail::access::data(arg).data;
}

template<class T, class Concept, class Tag>
bool check_any_cast(const any<Concept, Tag>&, ::lslboost::mpl::true_)
{
    return true;
}

template<class T, class Concept, class Tag>
bool check_any_cast(const any<Concept, Tag>& arg, ::lslboost::mpl::false_)
{
    typedef typename ::lslboost::remove_cv<
        typename ::lslboost::remove_reference<Tag>::type
    >::type tag_type;
    return ::lslboost::type_erasure::detail::access::table(arg)
        .template find<typeid_<tag_type> >()() == typeid(T);
}

template<class T, class Concept, class Tag>
bool check_any_cast(const any<Concept, Tag>& arg)
{
    return ::lslboost::type_erasure::detail::check_any_cast<T>(
        arg, ::lslboost::is_void<typename ::lslboost::remove_reference<T>::type>());
}

}

/**
 * Attempts to extract the object that @c arg holds.
 * If casting to a pointer fails, \any_cast returns
 * a null pointer.  Casting to @c void* always succeeds
 * and returns the address of stored object.
 *
 * \code
 * any<mpl::vector<typeid_<>, copy_constructible<> > > x(1);
 * any_cast<int>(x);      // returns 1
 * any_cast<int&>(x);     // returns a reference to the contents of x
 * any_cast<double>(x);   // throws bad_any_cast
 * any_cast<int*>(&x);    // returns a pointer to the contents of x
 * any_cast<void*>(&x);   // returns a pointer to the contents of x
 * any_cast<double*>(&x); // returns NULL
 * \endcode
 *
 * \pre if @c arg is a pointer, @c T must be a pointer type.
 * \pre @c Concept must contain @ref typeid_<tt>&lt;Tag&gt;</tt>.
 *
 * \throws bad_any_cast if @c arg doesn't contain
 *         an object of type @c T and we're casting
 *         to a value or reference.
 */
template<class T, class Concept, class Tag>
T any_cast(any<Concept, Tag>& arg)
{
    if(::lslboost::type_erasure::detail::check_any_cast<T>(arg)) {
        return *static_cast<
            typename ::lslboost::remove_reference<
                typename ::lslboost::add_const<T>::type
            >::type*
        >(::lslboost::type_erasure::detail::get_pointer(arg));
    } else {
        BOOST_THROW_EXCEPTION(::lslboost::type_erasure::bad_any_cast());
    }
}

/** \overload */
template<class T, class Concept, class Tag>
T any_cast(const any<Concept, Tag>& arg)
{
    if(::lslboost::type_erasure::detail::check_any_cast<T>(arg)) {
        return *static_cast<
            typename ::lslboost::remove_reference<
                typename ::lslboost::add_const<T>::type
            >::type*
        >(::lslboost::type_erasure::detail::get_pointer(arg));
    } else {
        BOOST_THROW_EXCEPTION(::lslboost::type_erasure::bad_any_cast());
    }
}

/** \overload */
template<class T, class Concept, class Tag>
T any_cast(any<Concept, Tag>* arg)
{
    BOOST_MPL_ASSERT((::lslboost::is_pointer<T>));
    if(::lslboost::type_erasure::detail::check_any_cast<
        typename ::lslboost::remove_pointer<T>::type>(*arg)) {
        return static_cast<
            typename ::lslboost::remove_pointer<T>::type*>(
                ::lslboost::type_erasure::detail::get_pointer(*arg));
    } else {
        return 0;
    }
}

/** \overload */
template<class T, class Concept, class Tag>
T any_cast(const any<Concept, Tag>* arg)
{
    BOOST_MPL_ASSERT((::lslboost::is_pointer<T>));
    if(::lslboost::type_erasure::detail::check_any_cast<
        typename ::lslboost::remove_pointer<T>::type>(*arg)) {
        return static_cast<
            typename ::lslboost::remove_pointer<T>::type*>(
                ::lslboost::type_erasure::detail::get_pointer(*arg));
    } else {
        return 0;
    }
}

}
}

#endif
