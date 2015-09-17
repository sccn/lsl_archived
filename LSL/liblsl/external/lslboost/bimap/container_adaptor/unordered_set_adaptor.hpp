// Boost.Bimap
//
// Copyright (c) 2006-2007 Matias Capeletto
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

/// \file container_adaptor/unordered_set_adaptor.hpp
/// \brief Container adaptor to easily build a std::unordered_set signature compatible container.

#ifndef BOOST_BIMAP_CONTAINER_ADAPTOR_UNORDERED_SET_ADAPTOR_HPP
#define BOOST_BIMAP_CONTAINER_ADAPTOR_UNORDERED_SET_ADAPTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER>=1200)
#pragma once
#endif

#include <lslboost/config.hpp>

#include <lslboost/bimap/container_adaptor/unordered_associative_container_adaptor.hpp>
#include <lslboost/mpl/aux_/na.hpp>
#include <lslboost/mpl/vector.hpp>

namespace lslboost {
namespace bimaps {
namespace container_adaptor {

/// \brief Container adaptor to easily build a std::unordered_set signature compatible container.

template
<
    class Base,

    class Iterator,
    class ConstIterator,
    class LocalIterator,
    class ConstLocalIterator,

    class IteratorToBaseConverter        = ::lslboost::mpl::na,
    class IteratorFromBaseConverter      = ::lslboost::mpl::na,
    class LocalIteratorFromBaseConverter = ::lslboost::mpl::na,
    class ValueToBaseConverter           = ::lslboost::mpl::na,
    class ValueFromBaseConverter         = ::lslboost::mpl::na,
    class KeyToBaseConverter             = ::lslboost::mpl::na,

    class FunctorsFromDerivedClasses = mpl::vector<>
>
class unordered_set_adaptor :

    public ::lslboost::bimaps::container_adaptor::
                unordered_associative_container_adaptor
    <
        Base,
        Iterator, ConstIterator, LocalIterator, ConstLocalIterator,
        BOOST_DEDUCED_TYPENAME Iterator::value_type,
        IteratorToBaseConverter, IteratorFromBaseConverter,
        LocalIteratorFromBaseConverter,
        ValueToBaseConverter, ValueFromBaseConverter,
        KeyToBaseConverter,
        FunctorsFromDerivedClasses
    >
{
    typedef ::lslboost::bimaps::container_adaptor::
                unordered_associative_container_adaptor
    <
        Base,
        Iterator, ConstIterator, LocalIterator, ConstLocalIterator,
        BOOST_DEDUCED_TYPENAME Iterator::value_type,
        IteratorToBaseConverter, IteratorFromBaseConverter,
        LocalIteratorFromBaseConverter,
        ValueToBaseConverter, ValueFromBaseConverter,
        KeyToBaseConverter,
        FunctorsFromDerivedClasses

    > base_;

    // Access -----------------------------------------------------------------

    public:

    explicit unordered_set_adaptor(Base & c) :
        base_(c) {}

    protected:

    typedef unordered_set_adaptor unordered_set_adaptor_;

};


} // namespace container_adaptor
} // namespace bimaps
} // namespace lslboost


#endif // BOOST_BIMAP_CONTAINER_ADAPTOR_UNORDERED_SET_ADAPTOR_HPP

