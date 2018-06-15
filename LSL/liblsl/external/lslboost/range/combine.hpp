//  Copyright Neil Groves 2010. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_COMBINE_HPP
#define BOOST_RANGE_COMBINE_HPP

#include <lslboost/iterator/zip_iterator.hpp>
#include <lslboost/tuple/tuple.hpp>
#include <lslboost/range/iterator.hpp>
#include <lslboost/range/iterator_range.hpp>
#include <lslboost/type_traits/is_void.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/int.hpp>
#include <lslboost/mpl/plus.hpp>
#include <lslboost/mpl/arithmetic.hpp>
#include <lslboost/config.hpp>

namespace lslboost
{
    namespace range_detail
    {
        struct void_ { typedef void_ type; };
    }

    template<> struct range_iterator< ::lslboost::range_detail::void_ >
    {
       typedef ::lslboost::tuples::null_type type;
    };

    namespace range_detail
    {
        inline ::lslboost::tuples::null_type range_begin( ::lslboost::range_detail::void_& )
        { return ::lslboost::tuples::null_type(); }

        inline ::lslboost::tuples::null_type range_begin( const ::lslboost::range_detail::void_& )
        { return ::lslboost::tuples::null_type(); }

        inline ::lslboost::tuples::null_type range_end( ::lslboost::range_detail::void_& )
        { return ::lslboost::tuples::null_type(); }

        inline ::lslboost::tuples::null_type range_end( const ::lslboost::range_detail::void_& )
        { return ::lslboost::tuples::null_type(); }

        template< class T >
        struct tuple_iter
        {
            typedef BOOST_DEDUCED_TYPENAME ::lslboost::mpl::eval_if_c<
                ::lslboost::is_same<T, ::lslboost::range_detail::void_ >::value,
                ::lslboost::mpl::identity< ::lslboost::tuples::null_type >,
                ::lslboost::range_iterator<T>
            >::type type;
        };

        template< class Rng1, class Rng2 >
        struct tuple_range
        {
            typedef BOOST_DEDUCED_TYPENAME ::lslboost::mpl::eval_if_c<
                ::lslboost::is_same<Rng1, ::lslboost::range_detail::void_ >::value,
                ::lslboost::range_detail::void_,
                ::lslboost::mpl::identity<Rng1>
            >::type type;
        };

        template
        <
            class R1,
            class R2,
            class R3,
            class R4,
            class R5,
            class R6
        >
        struct generate_tuple
        {
            typedef ::lslboost::tuples::tuple<
                        BOOST_DEDUCED_TYPENAME tuple_iter<R1>::type,
                        BOOST_DEDUCED_TYPENAME tuple_iter<R2>::type,
                        BOOST_DEDUCED_TYPENAME tuple_iter<R3>::type,
                        BOOST_DEDUCED_TYPENAME tuple_iter<R4>::type,
                        BOOST_DEDUCED_TYPENAME tuple_iter<R5>::type,
                        BOOST_DEDUCED_TYPENAME tuple_iter<R6>::type
                    > type;

            static type begin( R1& r1, R2& r2, R3& r3, R4& r4, R5& r5, R6& r6 )
            {
                return ::lslboost::tuples::make_tuple( ::lslboost::begin(r1),
                                                    ::lslboost::begin(r2),
                                                    ::lslboost::begin(r3),
                                                    ::lslboost::begin(r4),
                                                    ::lslboost::begin(r5),
                                                    ::lslboost::begin(r6) );
            }

            static type end( R1& r1, R2& r2, R3& r3, R4& r4, R5& r5, R6& r6 )
            {
                return ::lslboost::tuples::make_tuple( ::lslboost::end(r1),
                                                    ::lslboost::end(r2),
                                                    ::lslboost::end(r3),
                                                    ::lslboost::end(r4),
                                                    ::lslboost::end(r5),
                                                    ::lslboost::end(r6) );
            }
        };

        template
        <
            class R1,
            class R2 = void_,
            class R3 = void_,
            class R4 = void_,
            class R5 = void_,
            class R6 = void_
        >
        struct zip_rng
            : iterator_range<
                zip_iterator<
                    BOOST_DEDUCED_TYPENAME generate_tuple<R1,R2,R3,R4,R5,R6>::type
                >
            >
        {
        private:
            typedef generate_tuple<R1,R2,R3,R4,R5,R6>        generator_t;
            typedef BOOST_DEDUCED_TYPENAME generator_t::type tuple_t;
            typedef zip_iterator<tuple_t>                    zip_iter_t;
            typedef iterator_range<zip_iter_t>               base_t;

        public:
            zip_rng( R1& r1, R2& r2, R3& r3, R4& r4, R5& r5, R6& r6 )
            : base_t( zip_iter_t( generator_t::begin(r1,r2,r3,r4,r5,r6) ),
                      zip_iter_t( generator_t::end(r1,r2,r3,r4,r5,r6) ) )
            {
                BOOST_ASSERT(::lslboost::distance(r1) <= ::lslboost::distance(r2));
                BOOST_ASSERT(::lslboost::distance(r1) <= ::lslboost::distance(r3));
                BOOST_ASSERT(::lslboost::distance(r1) <= ::lslboost::distance(r4));
                BOOST_ASSERT(::lslboost::distance(r1) <= ::lslboost::distance(r5));
                BOOST_ASSERT(::lslboost::distance(r1) <= ::lslboost::distance(r6));
            }

            template< class Zip, class Rng >
            zip_rng( Zip& z, Rng& r )
            : base_t( zip_iter_t( generator_t::begin( z, r ) ),
                      zip_iter_t( generator_t::end( z, r ) ) )
            {

                // @todo: tuple::begin( should be overloaded for this situation
            }

            struct tuple_length : ::lslboost::tuples::length<tuple_t>
            { };

            template< unsigned N >
            struct get
            {
                template< class Z, class R >
                static BOOST_DEDUCED_TYPENAME ::lslboost::tuples::element<N,tuple_t>::type begin( Z& z, R& )
                {
                    return get<N>( z.begin().get_iterator_tuple() );
                }

                template< class Z, class R >
                static BOOST_DEDUCED_TYPENAME ::lslboost::tuples::element<N,tuple_t>::type end( Z& z, R& r )
                {
                    return get<N>( z.end().get_iterator_tuple() );
                }
            };

        };

        template< class Rng1, class Rng2 >
        struct zip_range
            : iterator_range<
                zip_iterator<
                    ::lslboost::tuples::tuple<
                        BOOST_DEDUCED_TYPENAME ::lslboost::range_iterator<Rng1>::type,
                        BOOST_DEDUCED_TYPENAME ::lslboost::range_iterator<Rng2>::type
                    >
                >
            >
        {
        private:
            typedef zip_iterator<
                        ::lslboost::tuples::tuple<
                            BOOST_DEDUCED_TYPENAME ::lslboost::range_iterator<Rng1>::type,
                            BOOST_DEDUCED_TYPENAME ::lslboost::range_iterator<Rng2>::type
                        >
                    > zip_iter_t;
            typedef iterator_range<zip_iter_t> base_t;

        public:
            zip_range( Rng1& r1, Rng2& r2 )
            : base_t( zip_iter_t( ::lslboost::tuples::make_tuple(::lslboost::begin(r1),
                                                              ::lslboost::begin(r2)) ),
                      zip_iter_t( ::lslboost::tuples::make_tuple(::lslboost::end(r1),
                                                              ::lslboost::end(r2)) ) )
            {
                BOOST_ASSERT(::lslboost::distance(r1) <= ::lslboost::distance(r2));
            }
        };

        template< class Rng1, class Rng2, class Rng3 >
        struct zip_range3
            : iterator_range<
                zip_iterator<
                    ::lslboost::tuples::tuple<
                        BOOST_DEDUCED_TYPENAME ::lslboost::range_iterator<Rng1>::type,
                        BOOST_DEDUCED_TYPENAME ::lslboost::range_iterator<Rng2>::type,
                        BOOST_DEDUCED_TYPENAME ::lslboost::range_iterator<Rng3>::type
                    >
                >
            >
        {
        private:
            typedef zip_iterator<
                ::lslboost::tuples::tuple<
                    BOOST_DEDUCED_TYPENAME ::lslboost::range_iterator<Rng1>::type,
                    BOOST_DEDUCED_TYPENAME ::lslboost::range_iterator<Rng2>::type,
                    BOOST_DEDUCED_TYPENAME ::lslboost::range_iterator<Rng3>::type
                >
            > zip_iter_t;
            typedef iterator_range<zip_iter_t> base_t;

        public:
            zip_range3( Rng1& r1, Rng2& r2, Rng3& r3 )
            : base_t( zip_iter_t( ::lslboost::tuples::make_tuple(::lslboost::begin(r1),
                                                              ::lslboost::begin(r2),
                                                              ::lslboost::begin(r3)) ),
                      zip_iter_t( ::lslboost::tuples::make_tuple(::lslboost::end(r1),
                                                              ::lslboost::end(r2),
                                                              ::lslboost::end(r3)) )
                    )
            {
                BOOST_ASSERT(::lslboost::distance(r1) <= ::lslboost::distance(r2));
                BOOST_ASSERT(::lslboost::distance(r1) <= ::lslboost::distance(r3));
            }
        };


        struct combine_tag {};

        template< class Rng >
        inline zip_rng<Rng>
        operator&( combine_tag, Rng& r )
        {
            return zip_rng<Rng>(r);
        }

        template< class Rng >
        inline iterator_range<const Rng>
        operator&( combine_tag, const Rng& r )
        {
            return iterator_range<const Rng>(r);
        }

        template
        <
            class R1,
            class R2,
            class R3,
            class R4,
            class R5,
            class Rng
        >
        inline BOOST_DEDUCED_TYPENAME zip_rng<R1,R2,R3,R4,R5>::next
        operator&( const zip_rng<R1,R2,R3,R4,R5>& zip,
                   Rng& r )
        {
            return zip_rng<R1,R2,R3,R4,R5>::next( zip, r );
        }

    } // namespace range_detail

    template< class Rng1, class Rng2 >
    inline ::lslboost::range_detail::zip_range<Rng1, Rng2> combine( Rng1& r1, Rng2& r2 )
    {
        return ::lslboost::range_detail::zip_range<Rng1, Rng2>(r1, r2);
    }

    template< class Rng1, class Rng2 >
    inline ::lslboost::range_detail::zip_range<const Rng1, Rng2> combine( const Rng1& r1, Rng2& r2 )
    {
        return ::lslboost::range_detail::zip_range<const Rng1, Rng2>(r1, r2);
    }

    template< class Rng1, class Rng2 >
    inline ::lslboost::range_detail::zip_range<Rng1, const Rng2> combine( Rng1& r1, const Rng2& r2 )
    {
        return ::lslboost::range_detail::zip_range<Rng1, const Rng2>(r1, r2);
    }

    template< class Rng1, class Rng2 >
    inline ::lslboost::range_detail::zip_range<const Rng1, const Rng2> combine( const Rng1& r1, const Rng2& r2 )
    {
        return ::lslboost::range_detail::zip_range<const Rng1, const Rng2>(r1, r2);
    }

} // namespace lslboost

#endif
