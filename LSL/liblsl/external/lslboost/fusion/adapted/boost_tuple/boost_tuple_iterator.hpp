/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_BOOST_TUPLE_ITERATOR_09262006_1851)
#define FUSION_BOOST_TUPLE_ITERATOR_09262006_1851

#include <lslboost/fusion/iterator/iterator_facade.hpp>
#include <lslboost/type_traits/is_const.hpp>
#include <lslboost/type_traits/add_const.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/or.hpp>
#include <lslboost/mpl/plus.hpp>
#include <lslboost/mpl/int.hpp>
#include <lslboost/mpl/apply.hpp>
#include <lslboost/tuple/tuple.hpp>

namespace lslboost { namespace fusion
{
    struct forward_traversal_tag;

    namespace detail
    {
        template <typename T>
        struct lslboost_tuple_is_empty : mpl::false_ {};

        template <>
        struct lslboost_tuple_is_empty<tuples::null_type> : mpl::true_ {};

        template <>
        struct lslboost_tuple_is_empty<tuples::null_type const> : mpl::true_ {};

        template <>
        struct lslboost_tuple_is_empty<tuples::tuple<> > : mpl::true_ {};

        template <>
        struct lslboost_tuple_is_empty<tuples::tuple<> const> : mpl::true_ {};
    }

    template <typename Cons = tuples::null_type>
    struct lslboost_tuple_iterator
        : iterator_facade<lslboost_tuple_iterator<Cons>, forward_traversal_tag>
    {
        typedef Cons cons_type;

        explicit lslboost_tuple_iterator(Cons& in_cons)
            : cons(in_cons) {}
        Cons& cons;

        template <typename Iterator>
        struct value_of : mpl::identity<typename Iterator::cons_type::head_type> {};

        template <typename Iterator>
        struct deref
        {
            typedef typename value_of<Iterator>::type element;

            typedef typename
                mpl::if_<
                    is_const<typename Iterator::cons_type>
                  , typename tuples::access_traits<element>::const_type
                  , typename tuples::access_traits<element>::non_const_type
                >::type
            type;

            static type
            call(Iterator const& iter)
            {
                return iter.cons.get_head();
            }
        };

        template <typename Iterator>
        struct next
        {
            typedef typename Iterator::cons_type cons_type;
            typedef typename cons_type::tail_type tail_type;

            typedef lslboost_tuple_iterator<
                typename mpl::eval_if<
                    is_const<cons_type>
                  , add_const<tail_type>
                  , mpl::identity<tail_type>
                >::type>
            type;

            static type
            call(Iterator const& iter)
            {
                return type(iter.cons.get_tail());
            }
        };
        
        template <typename I1, typename I2>
        struct distance;

        // detail
        template <typename I1, typename I2>
        struct lazy_next_distance
        {
            typedef
                typename mpl::plus<
                    mpl::int_<1>,
                    typename distance<
                        typename next<I1>::type,
                        I2
                    >::type
                >::type type;
        };
        
        template <typename I1, typename I2>
        struct distance
        {
            typedef typename mpl::eval_if<
                lslboost::is_same<I1, I2>,
                mpl::int_<0>,
                lazy_next_distance<I1, I2>
            >::type type;
            
            static type
            call(I1 const&, I2 const&)
            {
                return type();
            }
        };

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        lslboost_tuple_iterator& operator= (lslboost_tuple_iterator const&);
    };

    template <typename Null>
    struct lslboost_tuple_null_iterator
        : iterator_facade<lslboost_tuple_iterator<Null>, forward_traversal_tag>
    {
        typedef Null cons_type;

        template <typename I1, typename I2>
        struct equal_to
            : mpl::or_<
                is_same<I1, I2>
              , mpl::and_<
                    detail::lslboost_tuple_is_empty<typename I1::cons_type>
                  , detail::lslboost_tuple_is_empty<typename I2::cons_type>
                >
            >
        {};
    };

    template <>
    struct lslboost_tuple_iterator<tuples::null_type>
        : lslboost_tuple_null_iterator<tuples::null_type>
    {
        template <typename Cons>
        explicit lslboost_tuple_iterator(Cons const&) {}
    };

    template <>
    struct lslboost_tuple_iterator<tuples::null_type const>
        : lslboost_tuple_null_iterator<tuples::null_type const>
    {
        template <typename Cons>
        explicit lslboost_tuple_iterator(Cons const&) {}
    };

    template <>
    struct lslboost_tuple_iterator<tuples::tuple<> >
        : lslboost_tuple_null_iterator<tuples::tuple<> >
    {
        template <typename Cons>
        explicit lslboost_tuple_iterator(Cons const&) {}
    };

    template <>
    struct lslboost_tuple_iterator<tuples::tuple<> const>
        : lslboost_tuple_null_iterator<tuples::tuple<> const>
    {
        template <typename Cons>
        explicit lslboost_tuple_iterator(Cons const&) {}
    };
}}

#endif


