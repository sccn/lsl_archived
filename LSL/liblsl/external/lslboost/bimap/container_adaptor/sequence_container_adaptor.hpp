// Boost.Bimap
//
// Copyright (c) 2006-2007 Matias Capeletto
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

/// \file container_adaptor/sequence_container_adaptor.hpp
/// \brief Container adaptor to build a type that is compliant to the concept of a weak associative container.

#ifndef BOOST_BIMAP_CONTAINER_ADAPTOR_SEQUENCE_CONTAINER_ADAPTOR_HPP
#define BOOST_BIMAP_CONTAINER_ADAPTOR_SEQUENCE_CONTAINER_ADAPTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER>=1200)
#pragma once
#endif

#include <lslboost/config.hpp>

#include <utility>

#include <lslboost/mpl/if.hpp>
#include <lslboost/mpl/vector.hpp>
#include <lslboost/mpl/aux_/na.hpp>
#include <lslboost/bimap/container_adaptor/detail/identity_converters.hpp>
#include <lslboost/bimap/container_adaptor/container_adaptor.hpp>
#include <lslboost/call_traits.hpp>
#include <lslboost/operators.hpp>

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
    class FunctorsFromDerivedClasses
>
struct sequence_container_adaptor_base
{
    typedef container_adaptor
    <
        Base, Iterator, ConstIterator,
        IteratorToBaseConverter, IteratorFromBaseConverter,
        ValueToBaseConverter, ValueFromBaseConverter,

        BOOST_DEDUCED_TYPENAME mpl::push_front<

            FunctorsFromDerivedClasses,

            BOOST_DEDUCED_TYPENAME mpl::if_<
                ::lslboost::mpl::is_na<ReverseIteratorFromBaseConverter>,
            // {
                    detail::iterator_from_base_identity
                    <
                        BOOST_DEDUCED_TYPENAME Base::reverse_iterator, 
                        ReverseIterator,
                        BOOST_DEDUCED_TYPENAME Base::const_reverse_iterator,
                        ConstReverseIterator
                    >,
            // }
            // else
            // {
                    ReverseIteratorFromBaseConverter
            // }

            >::type

        >::type

    > type;
};

#endif // BOOST_BIMAP_DOXYGEN_WILL_NOT_PROCESS_THE_FOLLOWING_LINES

/// \brief Container adaptor to build a type that is compliant to the concept of a sequence container.

template
<
    class Base,

    class Iterator,
    class ConstIterator,

    class ReverseIterator,
    class ConstReverseIterator,

    class IteratorToBaseConverter           = ::lslboost::mpl::na,
    class IteratorFromBaseConverter         = ::lslboost::mpl::na,
    class ReverseIteratorFromBaseConverter  = ::lslboost::mpl::na,
    class ValueToBaseConverter              = ::lslboost::mpl::na,
    class ValueFromBaseConverter            = ::lslboost::mpl::na,

    class FunctorsFromDerivedClasses = mpl::vector<>
>
class sequence_container_adaptor :

    public sequence_container_adaptor_base
    <
        Base, Iterator, ConstIterator,
        ReverseIterator, ConstReverseIterator,
        IteratorToBaseConverter, IteratorFromBaseConverter,
        ReverseIteratorFromBaseConverter,
        ValueToBaseConverter, ValueFromBaseConverter,
        FunctorsFromDerivedClasses

    >::type,

    ::lslboost::totally_ordered
    <
        sequence_container_adaptor
        <
            Base, Iterator, ConstIterator,
            ReverseIterator, ConstReverseIterator,
            IteratorToBaseConverter, IteratorFromBaseConverter,
            ReverseIteratorFromBaseConverter,
            ValueToBaseConverter, ValueFromBaseConverter,
            FunctorsFromDerivedClasses
        >
    >
{
    typedef BOOST_DEDUCED_TYPENAME sequence_container_adaptor_base
    <
        Base, Iterator, ConstIterator,
        ReverseIterator, ConstReverseIterator,
        IteratorToBaseConverter, IteratorFromBaseConverter,
        ReverseIteratorFromBaseConverter,
        ValueToBaseConverter, ValueFromBaseConverter,
        FunctorsFromDerivedClasses

    >::type base_;

    // MetaData -------------------------------------------------------------

    public:

    typedef ReverseIterator      reverse_iterator;
    typedef ConstReverseIterator const_reverse_iterator;

    protected:

    typedef BOOST_DEDUCED_TYPENAME mpl::if_<
        ::lslboost::mpl::is_na<ReverseIteratorFromBaseConverter>,
        // {
                detail::iterator_from_base_identity
                <
                    BOOST_DEDUCED_TYPENAME Base::reverse_iterator,
                    reverse_iterator,
                    BOOST_DEDUCED_TYPENAME Base::const_reverse_iterator,
                    const_reverse_iterator
                >,
        // }
        // else
        // {
                ReverseIteratorFromBaseConverter
        // }

        >::type reverse_iterator_from_base;


    // Access -----------------------------------------------------------------

    public:

    explicit sequence_container_adaptor(Base & c)
        : base_(c) {}

    protected:


    typedef sequence_container_adaptor sequence_container_adaptor_;

    // Interface --------------------------------------------------------------

    public:

    reverse_iterator rbegin()
    {
        return this->template functor<
            reverse_iterator_from_base
        >()                            ( this->base().rbegin() );

    }

    reverse_iterator rend()
    {
        return this->template functor<
            reverse_iterator_from_base
        >()                            ( this->base().rend() );
    }

    const_reverse_iterator rbegin() const
    {
        return this->template functor<
            reverse_iterator_from_base
        >()                            ( this->base().rbegin() );
    }

    const_reverse_iterator rend() const
    {
        return this->template functor<
            reverse_iterator_from_base
        >()                            ( this->base().rend() );
    }

    void resize(BOOST_DEDUCED_TYPENAME base_::size_type n,
                BOOST_DEDUCED_TYPENAME ::lslboost::call_traits<
                    BOOST_DEDUCED_TYPENAME base_::value_type >::param_type x =
                        BOOST_DEDUCED_TYPENAME base_::value_type())
    {
        this->base().resize(n,
            this->template functor<BOOST_DEDUCED_TYPENAME base_::value_to_base>()(x)
        );
    }

    BOOST_DEDUCED_TYPENAME base_::reference front()
    {
        return this->template functor<
            BOOST_DEDUCED_TYPENAME base_::value_from_base>()
        (
            this->base().front()
        );
    }

    BOOST_DEDUCED_TYPENAME base_::reference back()
    {
        return this->template functor<
            BOOST_DEDUCED_TYPENAME base_::value_from_base>()
        (
            this->base().back()
        );
    }

    BOOST_DEDUCED_TYPENAME base_::const_reference front() const
    {
        return this->template functor<
            BOOST_DEDUCED_TYPENAME base_::value_from_base>()
        (
            this->base().front()
        );
    }

    BOOST_DEDUCED_TYPENAME base_::const_reference back() const
    {
        return this->template functor<
            BOOST_DEDUCED_TYPENAME base_::value_from_base>()
        (
            this->base().back()
        );
    }

    void push_front(
        BOOST_DEDUCED_TYPENAME ::lslboost::call_traits<
            BOOST_DEDUCED_TYPENAME base_::value_type >::param_type x)
    {
        this->base().push_front(
            this->template functor<BOOST_DEDUCED_TYPENAME base_::value_to_base>()(x));
    }

    void pop_front()
    {
        this->base().pop_front();
    }

    void push_back(
        BOOST_DEDUCED_TYPENAME ::lslboost::call_traits< 
            BOOST_DEDUCED_TYPENAME base_::value_type >::param_type x)
    {
        this->base().push_back(
            this->template functor<BOOST_DEDUCED_TYPENAME base_::value_to_base>()(x));
    }

    void pop_back()
    {
        this->base().pop_back();
    }

    std::pair<BOOST_DEDUCED_TYPENAME base_::iterator,bool>
    insert(BOOST_DEDUCED_TYPENAME base_::iterator position,
           BOOST_DEDUCED_TYPENAME ::lslboost::call_traits< 
                BOOST_DEDUCED_TYPENAME base_::value_type >::param_type x)
    {
        std::pair< BOOST_DEDUCED_TYPENAME Base::iterator, bool > r(
            this->base().insert(
                this->template functor<
                    BOOST_DEDUCED_TYPENAME base_::iterator_to_base>()(position),
                this->template functor<
                    BOOST_DEDUCED_TYPENAME base_::value_to_base   >()(x)
            )
        );

        return std::pair<BOOST_DEDUCED_TYPENAME base_::iterator, bool>(
            this->template functor<
                BOOST_DEDUCED_TYPENAME base_::iterator_from_base>()(r.first),
            r.second
        );
    }

    void insert(BOOST_DEDUCED_TYPENAME base_::iterator position,
                BOOST_DEDUCED_TYPENAME base_::size_type m,
                BOOST_DEDUCED_TYPENAME ::lslboost::call_traits<
                    BOOST_DEDUCED_TYPENAME base_::value_type >::param_type x)
    {
        this->base().insert(
            this->template functor<
                BOOST_DEDUCED_TYPENAME base_::iterator_to_base>()(position),
            m,
            this->template functor<BOOST_DEDUCED_TYPENAME base_::value_to_base   >()(x)
        );
    }

    template< class InputIterator >
    void insert(BOOST_DEDUCED_TYPENAME base_::iterator position,
                InputIterator first, InputIterator last)
    {
        // This is the same problem found in the insert function 
        // of container_adaptor
        // For now, do the simple thing. This can be optimized

        for( ; first != last ; ++first )
        {
            this->base().insert(
                this->template functor<
                    BOOST_DEDUCED_TYPENAME base_::iterator_to_base>()( position ),
                this->template functor<
                    BOOST_DEDUCED_TYPENAME base_::value_to_base   >()( *first )
            );
        }
    }

    // Totally ordered implementation

    bool operator==(const sequence_container_adaptor & c) const
    {
        return ( this->base() == c.base() );
    }

    bool operator<(const sequence_container_adaptor & c) const
    {
        return ( this->base() < c.base() );
    }
};

} // namespace container_adaptor
} // namespace bimaps
} // namespace lslboost


#endif // BOOST_BIMAP_CONTAINER_ADAPTOR_SEQUENCE_CONTAINER_ADAPTOR_HPP
