// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_ITERATOR_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_ITERATOR_HPP_INCLUDED

#include <iterator>
#include <lslboost/mpl/vector.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/iterator/iterator_categories.hpp>
#include <lslboost/type_traits/remove_const.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/type_erasure/operators.hpp>
#include <lslboost/type_erasure/builtin.hpp>
#include <lslboost/type_erasure/deduced.hpp>
#include <lslboost/type_erasure/is_placeholder.hpp>

namespace lslboost {

struct use_default;

namespace type_erasure {

namespace detail {

template<class T>
struct iterator_value_type_impl
{
    typedef typename ::std::iterator_traits<T>::value_type type;
};

}

/** INTERNAL ONLY */
template<class T>
struct iterator_value_type
{
    typedef typename ::lslboost::mpl::eval_if<
        ::lslboost::type_erasure::is_placeholder<T>,
        ::lslboost::mpl::identity<void>,
        ::lslboost::type_erasure::detail::iterator_value_type_impl<T>
    >::type type;
};

template<
    class Traversal,
    class T = _self,
    class Reference = ::lslboost::use_default,
    class DifferenceType = ::std::ptrdiff_t,
    class ValueType = typename deduced<iterator_value_type<T> >::type
>
struct iterator;

#ifdef BOOST_TYPE_ERASURE_DOXYGEN

/**
 * The @ref iterator concept can be used for any iterator category.
 *
 * \tparam Traversal must be one of @c lslboost::incrementable_traversal_tag,
 *         @c lslboost::single_pass_traversal_tag, @c lslboost::forward_traversal_tag,
 *         @c lslboost::bidirectional_traversal_tag, and @c lslboost::random_access_traversal_tag.
 * \tparam T The placeholder representing the iterator.
 * \tparam Reference The reference type.  If it is lslboost::use_default, then
 *         reference will be value_type&.
 * \tparam DifferenceType The iterator's difference type.
 *
 * The value_type of the iterator is deduced.  To force it to be
 * a specific type, use the @ref same_type concept.
 *
 * Example:
 *
 * \code
 * mpl::vector<
 *   iterator<lslboost::forward_traversal_tag>,
 *   same_type<iterator<lslboost::forward_traversal_tag>::value_type, int> > int_it;
 * \endcode
 */
template<
    class Traversal,
    class T = _self,
    class Reference = lslboost::use_default,
    class DifferenceType = std::ptrdiff_t
>
struct iterator
{
    typedef detail::unspecified value_type;
    typedef Reference reference;
    typedef DifferenceType difference_type;
};

template<
    class T = _self,
    class Reference = lslboost::use_default,
    class DifferenceType = std::ptrdiff_t
>
struct forward_iterator :
    iterator<lslboost::forward_traversal_tag, T, Reference, DifferenceType>
{};

template<
    class T = _self,
    class Reference = lslboost::use_default,
    class DifferenceType = std::ptrdiff_t
>
struct bidirectional_iterator :
    iterator<lslboost::bidirectional_traversal_tag, T, Reference, DifferenceType>
{};

template<
    class T = _self,
    class Reference = lslboost::use_default,
    class DifferenceType = std::ptrdiff_t
>
struct random_access_iterator :
    iterator<lslboost::random_access_traversal_tag, T, Reference, DifferenceType>
{
};

#else

/** INTERNAL ONLY */
template<class Reference, class ValueType>
struct iterator_reference
{
    typedef Reference type;
};

/** INTERNAL ONLY */
template<class ValueType>
struct iterator_reference< ::lslboost::use_default, ValueType>
{
    typedef ValueType& type;
};

template<class T, class Reference, class DifferenceType, class ValueType>
struct iterator< ::lslboost::no_traversal_tag, T, Reference, DifferenceType, ValueType> :
    lslboost::mpl::vector<
        copy_constructible<T>,
        constructible<T()>,
        equality_comparable<T>,
        dereferenceable<typename iterator_reference<Reference, ValueType>::type, T>,
        assignable<T>
    >
{
    typedef ValueType value_type;
    typedef typename iterator_reference<Reference, ValueType>::type reference;
    typedef DifferenceType difference_type;
};

template<class T, class Reference, class DifferenceType, class ValueType>
struct iterator< ::lslboost::incrementable_traversal_tag, T, Reference, DifferenceType, ValueType> :
    lslboost::mpl::vector<
        iterator< ::lslboost::no_traversal_tag, T, Reference, DifferenceType>,
        incrementable<T>
    >
{
    typedef ValueType value_type;
    typedef typename iterator_reference<Reference, ValueType>::type reference;
    typedef DifferenceType difference_type;
};

template<class T, class Reference, class DifferenceType, class ValueType>
struct iterator< ::lslboost::single_pass_traversal_tag, T, Reference, DifferenceType, ValueType> :
    iterator< ::lslboost::incrementable_traversal_tag, T, Reference, DifferenceType, ValueType>
{};

template<class T, class Reference, class DifferenceType, class ValueType>
struct iterator< ::lslboost::forward_traversal_tag, T, Reference, DifferenceType, ValueType> :
    iterator< ::lslboost::incrementable_traversal_tag, T, Reference, DifferenceType, ValueType>
{};

template<class T, class Reference, class DifferenceType, class ValueType>
struct iterator< ::lslboost::bidirectional_traversal_tag, T, Reference, DifferenceType, ValueType> :
    lslboost::mpl::vector<
        iterator< ::lslboost::incrementable_traversal_tag, T, Reference, DifferenceType, ValueType>,
        decrementable<T>
    >
{
    typedef ValueType value_type;
    typedef typename iterator_reference<Reference, ValueType>::type reference;
    typedef DifferenceType difference_type;
};

template<class T, class Reference, class DifferenceType, class ValueType>
struct iterator< ::lslboost::random_access_traversal_tag, T, Reference, DifferenceType, ValueType> :
    lslboost::mpl::vector<
        iterator< ::lslboost::bidirectional_traversal_tag, T, Reference, DifferenceType, ValueType>,
        addable<T, DifferenceType, T>,
        addable<DifferenceType, T, T>,
        subtractable<T, DifferenceType, T>,
        subtractable<T, T, DifferenceType>,
        subscriptable<typename iterator_reference<Reference, ValueType>::type, T, DifferenceType>
    >
{
    typedef ValueType value_type;
    typedef typename iterator_reference<Reference, ValueType>::type reference;
    typedef DifferenceType difference_type;
};

template<
    class T = _self,
    class Reference = ::lslboost::use_default,
    class DifferenceType = ::std::ptrdiff_t,
    class ValueType = typename deduced<iterator_value_type<T> >::type
>
struct forward_iterator :
    iterator< ::lslboost::forward_traversal_tag, T, Reference, DifferenceType, ValueType>
{};

template<
    class T = _self,
    class Reference = ::lslboost::use_default,
    class DifferenceType = ::std::ptrdiff_t,
    class ValueType = typename deduced<iterator_value_type<T> >::type
>
struct bidirectional_iterator :
    iterator< ::lslboost::bidirectional_traversal_tag, T, Reference, DifferenceType, ValueType>
{};

template<
    class T = _self,
    class Reference = ::lslboost::use_default,
    class DifferenceType = ::std::ptrdiff_t,
    class ValueType = typename deduced<iterator_value_type<T> >::type
>
struct random_access_iterator :
    iterator< ::lslboost::random_access_traversal_tag, T, Reference, DifferenceType, ValueType>
{
};

#endif

/// \cond show_operators

template<class T, class Reference, class DifferenceType, class ValueType, class Base>
struct concept_interface<iterator< ::lslboost::no_traversal_tag, T, Reference, DifferenceType, ValueType>, Base, T>
    : Base
{
    typedef typename rebind_any<Base, ValueType>::type value_type;
    typedef typename rebind_any<
        Base, 
        typename iterator_reference<Reference, ValueType>::type
    >::type reference;
    typedef DifferenceType difference_type;
    typedef typename ::lslboost::mpl::if_< ::lslboost::is_reference<reference>,
        typename ::lslboost::remove_reference<reference>::type*,
        value_type*
    >::type pointer;
};

template<class T, class Reference, class DifferenceType, class ValueType, class Base>
struct concept_interface<iterator< ::lslboost::forward_traversal_tag, T, Reference, DifferenceType, ValueType>, Base, T>
    : Base
{
    typedef std::forward_iterator_tag iterator_category;
};

template<class T, class Reference, class DifferenceType, class ValueType, class Base>
struct concept_interface<forward_iterator<T, Reference, DifferenceType, ValueType>, Base, T>
    : Base
{
    typedef std::forward_iterator_tag iterator_category;
};

template<class T, class Reference, class DifferenceType, class ValueType, class Base>
struct concept_interface<iterator< ::lslboost::bidirectional_traversal_tag, T, Reference, DifferenceType, ValueType>, Base, T>
    : Base
{
    typedef std::bidirectional_iterator_tag iterator_category;
};

template<class T, class Reference, class DifferenceType, class ValueType, class Base>
struct concept_interface<bidirectional_iterator<T, Reference, DifferenceType, ValueType>, Base, T>
    : Base
{
    typedef std::bidirectional_iterator_tag iterator_category;
};

template<class T, class Reference, class DifferenceType, class ValueType, class Base>
struct concept_interface<iterator< ::lslboost::random_access_traversal_tag, T, Reference, DifferenceType, ValueType>, Base, T>
    : Base
{
    typedef std::random_access_iterator_tag iterator_category;
};

template<class T, class Reference, class DifferenceType, class ValueType, class Base>
struct concept_interface<random_access_iterator<T, Reference, DifferenceType, ValueType>, Base, T>
    : Base
{
    typedef std::random_access_iterator_tag iterator_category;
};

/// \endcond

}
}

#endif
