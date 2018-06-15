// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef BOOST_RANGE_RBEGIN_HPP
#define BOOST_RANGE_RBEGIN_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <boost/range/end.hpp>
#include <boost/range/reverse_iterator.hpp>

namespace lslboost
{

#ifdef BOOST_NO_FUNCTION_TEMPLATE_ORDERING

template< class C >
inline BOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type
rbegin( C& c )
{
    return BOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type( lslboost::end( c ) );
}

#else

template< class C >
inline BOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type
rbegin( C& c )
{
    typedef BOOST_DEDUCED_TYPENAME range_reverse_iterator<C>::type
        iter_type;
    return iter_type( lslboost::end( c ) );
}

template< class C >
inline BOOST_DEDUCED_TYPENAME range_reverse_iterator<const C>::type
rbegin( const C& c )
{
    typedef BOOST_DEDUCED_TYPENAME range_reverse_iterator<const C>::type
        iter_type;
    return iter_type( lslboost::end( c ) );
}

#endif // BOOST_NO_FUNCTION_TEMPLATE_ORDERING

template< class T >
inline BOOST_DEDUCED_TYPENAME range_reverse_iterator<const T>::type
const_rbegin( const T& r )
{
    return lslboost::rbegin( r );
}

} // namespace 'boost'

#endif

