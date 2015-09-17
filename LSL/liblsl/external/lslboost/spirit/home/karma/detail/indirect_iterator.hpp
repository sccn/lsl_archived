//  Copyright (c) 2001-2011 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#if !defined(SPIRIT_KARMA_INDIRECT_ITERATOR_JAN_19_2011_0814PM)
#define SPIRIT_KARMA_INDIRECT_ITERATOR_JAN_19_2011_0814PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <lslboost/spirit/home/support/unused.hpp>
#include <lslboost/iterator/iterator_facade.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace lslboost { namespace spirit { namespace karma { namespace detail
{
    ///////////////////////////////////////////////////////////////////////
    // This is a wrapper for any iterator allowing to pass a reference of it
    // to the components of the sequence
    template <typename Iterator>
    class indirect_iterator
      : public lslboost::iterator_facade<
            indirect_iterator<Iterator>
          , typename lslboost::detail::iterator_traits<Iterator>::value_type
          , lslboost::forward_traversal_tag
          , typename lslboost::detail::iterator_traits<Iterator>::reference>
    {
        typedef typename lslboost::detail::iterator_traits<Iterator>::value_type
            base_value_type;
        typedef typename lslboost::detail::iterator_traits<Iterator>::reference
            base_reference_type;

        typedef lslboost::iterator_facade<
            indirect_iterator<Iterator>, base_value_type
          , lslboost::forward_traversal_tag, base_reference_type
        > base_type;

    public:
        indirect_iterator(Iterator& iter)
          : iter_(&iter)
        {}
        indirect_iterator(indirect_iterator const& iter)
          : iter_(iter.iter_)
        {}

    private:
        friend class lslboost::iterator_core_access;

        void increment()
        {
            ++*iter_;
        }

        bool equal(indirect_iterator const& other) const
        {
            return *iter_ == *other.iter_;
        }

        base_reference_type dereference() const
        {
            return **iter_;
        }

    private:
        Iterator* iter_;
    };
}}}}

///////////////////////////////////////////////////////////////////////////////
namespace lslboost { namespace spirit { namespace traits
{
    template <typename Iterator>
    struct make_indirect_iterator
    {
        typedef karma::detail::indirect_iterator<Iterator> type;
    };

    template <typename Iterator>
    struct make_indirect_iterator<karma::detail::indirect_iterator<Iterator> >
    {
        typedef karma::detail::indirect_iterator<Iterator> type;
    };

    template <>
    struct make_indirect_iterator<unused_type const*>
    {
        typedef unused_type const* type;
    };

    template <typename Iterator>
    struct make_indirect_iterator<Iterator const&>
      : make_indirect_iterator<Iterator const>
    {};
}}}

#endif
