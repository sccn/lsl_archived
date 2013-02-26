 // Boost.Bimap
//
// Copyright (c) 2006-2007 Matias Capeletto
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

/// \file container_adaptor/list_map_adaptor.hpp
/// \brief Container adaptor.

#ifndef BOOST_BIMAP_CONTAINER_ADAPTOR_LIST_MAP_ADAPTOR_HPP
#define BOOST_BIMAP_CONTAINER_ADAPTOR_LIST_MAP_ADAPTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER>=1200)
#pragma once
#endif

#include <lslboost/config.hpp>

#include <lslboost/mpl/list.hpp>
#include <lslboost/mpl/push_front.hpp>

#include <lslboost/bimap/container_adaptor/list_adaptor.hpp>
#include <lslboost/bimap/container_adaptor/detail/identity_converters.hpp>
#include <lslboost/bimap/container_adaptor/detail/key_extractor.hpp>
#include <lslboost/bimap/container_adaptor/detail/comparison_adaptor.hpp>
#include <lslboost/mpl/vector.hpp>
#include <lslboost/mpl/aux_/na.hpp>
#include <lslboost/mpl/if.hpp>

namespace lslboost {
namespace bimaps {
namespace container_adaptor {

#ifndef BOOST_BIMAP_DOXYGEN_WILL_NOT_PROCESS_THE_FOLLOWING_LINES

template
<
    class Base, class Iterator, class ConstIterator,
    class ReverseIterator, class ConstReverseIterator,
    class IteratorToBaseConverter, class IteratorFromBaseConverter,
    class ReverseIteratorFromBaseConverter,
    class ValueToBaseConverter, class ValueFromBaseConverter,
    class KeyFromBaseValueConverter,
    class FunctorsFromDerivedClasses
>
struct list_map_adaptor_base
{
    typedef list_adaptor
    <
        Base,

        Iterator, ConstIterator, ReverseIterator, ConstReverseIterator,

        IteratorToBaseConverter, IteratorFromBaseConverter,

        ReverseIteratorFromBaseConverter,

        ValueToBaseConverter, ValueFromBaseConverter,

        BOOST_DEDUCED_TYPENAME mpl::push_front<

            FunctorsFromDerivedClasses,

            BOOST_DEDUCED_TYPENAME mpl::if_< ::lslboost::mpl::is_na<KeyFromBaseValueConverter>,
            // {
                    detail::key_from_pair_extractor
                    <
                        BOOST_DEDUCED_TYPENAME Iterator::value_type 
                    >,
            // }
            // else
            // {
                    KeyFromBaseValueConverter
            // }

            >::type

        >::type

    > type;
};

#endif // BOOST_BIMAP_DOXYGEN_WILL_NOT_PROCESS_THE_FOLLOWING_LINES

/// \brief Container adaptor to easily build a list map container

template
<
    class Base,

    class Iterator,
    class ConstIterator,
    class ReverseIterator,
    class ConstReverseIterator,

    class IteratorToBaseConverter          = ::lslboost::mpl::na,
    class IteratorFromBaseConverter        = ::lslboost::mpl::na,
    class ReverseIteratorFromBaseConverter = ::lslboost::mpl::na,
    class ValueToBaseConverter             = ::lslboost::mpl::na,
    class ValueFromBaseConverter           = ::lslboost::mpl::na,
    class KeyFromBaseValueConverter        = ::lslboost::mpl::na,

    class FunctorsFromDerivedClasses = mpl::vector<>
>
class list_map_adaptor :

    public list_map_adaptor_base
    <
        Base, Iterator, ConstIterator, ReverseIterator, ConstReverseIterator,
        IteratorToBaseConverter, IteratorFromBaseConverter,
        ReverseIteratorFromBaseConverter,
        ValueToBaseConverter, ValueFromBaseConverter,
        KeyFromBaseValueConverter,
        FunctorsFromDerivedClasses

    >::type
{
    typedef BOOST_DEDUCED_TYPENAME list_map_adaptor_base
    <
        Base, Iterator, ConstIterator, ReverseIterator, ConstReverseIterator,
        IteratorToBaseConverter, IteratorFromBaseConverter,
        ReverseIteratorFromBaseConverter,
        ValueToBaseConverter, ValueFromBaseConverter,
        KeyFromBaseValueConverter,
        FunctorsFromDerivedClasses

    >::type base_;

    // MetaData -------------------------------------------------------------

    public:

    typedef BOOST_DEDUCED_TYPENAME Iterator::value_type::first_type  key_type;
    typedef BOOST_DEDUCED_TYPENAME Iterator::value_type::second_type data_type;
    typedef data_type mapped_type;

    protected:

    typedef BOOST_DEDUCED_TYPENAME mpl::if_< ::lslboost::mpl::is_na<KeyFromBaseValueConverter>,
    // {
            detail::key_from_pair_extractor< BOOST_DEDUCED_TYPENAME Iterator::value_type >,
    // }
    // else
    // {
            KeyFromBaseValueConverter
    // }

    >::type key_from_base_value;

    // Access -----------------------------------------------------------------

    public:

    explicit list_map_adaptor(Base & c) :
        base_(c) {}

    protected:

    typedef list_map_adaptor list_map_adaptor_;

    // Functions -------------------------------------------------------------

    public:

    // The following functions are overwritten in order to work 
    // with key_type instead of value_type

    template< class Predicate >
    void remove_if(Predicate pred)
    {
        this->base().remove_if(
            ::lslboost::bimaps::container_adaptor::detail::unary_check_adaptor
            <
                Predicate,
                BOOST_DEDUCED_TYPENAME Base::value_type,
                key_from_base_value

            >( pred, this->template functor<key_from_base_value>() )
        );
    }

    void unique()
    {
        this->base().unique(
            ::lslboost::bimaps::container_adaptor::detail::comparison_adaptor
            <
                std::equal_to<key_type>,
                BOOST_DEDUCED_TYPENAME Base::value_type,
                key_from_base_value

            >(
                std::equal_to<key_type>(),
                this->template functor<key_from_base_value>()
            )
        );
    }

    template< class BinaryPredicate >
    void unique(BinaryPredicate binary_pred)
    {
        this->base().unique(
            ::lslboost::bimaps::container_adaptor::detail::comparison_adaptor
            <
                BinaryPredicate,
                BOOST_DEDUCED_TYPENAME Base::value_type,
                key_from_base_value

            >( binary_pred, this->template functor<key_from_base_value>() )
        );
    }

    void merge(list_map_adaptor & x)
    {
        this->base().merge(x.base(),
            ::lslboost::bimaps::container_adaptor::detail::comparison_adaptor
            <
                std::less<key_type>,
                BOOST_DEDUCED_TYPENAME Base::value_type,
                key_from_base_value

            >(
                std::less<key_type>(),
                this->template functor<key_from_base_value>()
            )
        );
    }

    template< class Compare >
    void merge(list_map_adaptor & x, Compare comp)
    {
        this->base().merge(x.base(),
            ::lslboost::bimaps::container_adaptor::detail::comparison_adaptor
            <
                Compare,
                BOOST_DEDUCED_TYPENAME Base::value_type,
                key_from_base_value

            >( comp, this->template functor<key_from_base_value>() )
        );
    }

    void sort()
    {
        this->base().sort(
            ::lslboost::bimaps::container_adaptor::detail::comparison_adaptor
            <
                std::less<key_type>,
                BOOST_DEDUCED_TYPENAME Base::value_type,
                key_from_base_value

            >(
                std::less<key_type>(),
                this->template functor<key_from_base_value>()
            )
        );
    }

    template< class Compare >
    void sort(Compare comp)
    {
        this->base().sort(
            ::lslboost::bimaps::container_adaptor::detail::comparison_adaptor
            <
                Compare,
                BOOST_DEDUCED_TYPENAME Base::value_type,
                key_from_base_value

            >( comp, this->template functor<key_from_base_value>() )
        );
    }

};


} // namespace container_adaptor
} // namespace bimaps
} // namespace lslboost


#endif // BOOST_BIMAP_CONTAINER_ADAPTOR_LIST_MAP_ADAPTOR_HPP

