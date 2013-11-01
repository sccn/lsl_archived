// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//

#ifndef BOOST_RANGE_DETAIL_END_HPP
#define BOOST_RANGE_DETAIL_END_HPP

#include <lslboost/config.hpp> // BOOST_MSVC
#include <lslboost/detail/workaround.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, < 1300)
# include <lslboost/range/detail/vc6/end.hpp>
#else
# include <lslboost/range/detail/implementation_help.hpp>
# include <lslboost/range/iterator.hpp>
# include <lslboost/range/detail/common.hpp>
# if BOOST_WORKAROUND(BOOST_MSVC, < 1310)
#  include <lslboost/range/detail/remove_extent.hpp>
# endif

namespace lslboost
{
    namespace range_detail
    {
        template< typename T >
        struct range_end;

        //////////////////////////////////////////////////////////////////////
        // default
        //////////////////////////////////////////////////////////////////////

        template<>
        struct range_end<std_container_>
        {
            template< typename C >
            static BOOST_RANGE_DEDUCED_TYPENAME range_iterator<C>::type
            fun( C& c )
            {
                return c.end();
            };
        };

        //////////////////////////////////////////////////////////////////////
        // pair
        //////////////////////////////////////////////////////////////////////

        template<>
        struct range_end<std_pair_>
        {
            template< typename P >
            static BOOST_RANGE_DEDUCED_TYPENAME range_iterator<P>::type
            fun( const P& p )
            {
                return p.second;
            }
        };

        //////////////////////////////////////////////////////////////////////
        // array
        //////////////////////////////////////////////////////////////////////

        template<>
        struct range_end<array_>
        {
        #if !BOOST_WORKAROUND(BOOST_MSVC, < 1310)
            template< typename T, std::size_t sz >
            static T* fun( T BOOST_RANGE_ARRAY_REF()[sz] )
            {
                return lslboost::range_detail::array_end( lslboost_range_array );
            }
        #else
            template<typename T>
            static BOOST_RANGE_DEDUCED_TYPENAME remove_extent<T>::type* fun(T& t)
            {
                return t + remove_extent<T>::size;
            }
        #endif
        };

    } // namespace 'range_detail'

    namespace range_adl_barrier
    {
        template< typename C >
        inline BOOST_RANGE_DEDUCED_TYPENAME range_iterator<C>::type
        end( C& c )
        {
            return range_detail::range_end< BOOST_RANGE_DEDUCED_TYPENAME range_detail::range<C>::type >::fun( c );
        }
    } // namespace range_adl_barrier

} // namespace 'lslboost'

# endif // VC6
#endif
