// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//

#ifndef BOOST_RANGE_MUTABLE_ITERATOR_HPP
#define BOOST_RANGE_MUTABLE_ITERATOR_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <lslboost/range/config.hpp>

#include <lslboost/range/range_fwd.hpp>
#include <lslboost/range/detail/extract_optional_type.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/iterator/iterator_traits.hpp>
#include <cstddef>
#include <utility>

namespace lslboost
{

    //////////////////////////////////////////////////////////////////////////
    // default
    //////////////////////////////////////////////////////////////////////////
    
    namespace range_detail
    {

BOOST_RANGE_EXTRACT_OPTIONAL_TYPE( iterator )

template< typename C >
struct range_mutable_iterator
        : range_detail::extract_iterator<
            BOOST_DEDUCED_TYPENAME remove_reference<C>::type>
{};

//////////////////////////////////////////////////////////////////////////
// pair
//////////////////////////////////////////////////////////////////////////

template< typename Iterator >
struct range_mutable_iterator< std::pair<Iterator,Iterator> >
{
    typedef Iterator type;
};

//////////////////////////////////////////////////////////////////////////
// array
//////////////////////////////////////////////////////////////////////////

template< typename T, std::size_t sz >
struct range_mutable_iterator< T[sz] >
{
    typedef T* type;
};

    } // namespace range_detail

template<typename C, typename Enabler=void>
struct range_mutable_iterator
        : range_detail::range_mutable_iterator<
            BOOST_DEDUCED_TYPENAME remove_reference<C>::type
        >
{
};

} // namespace lslboost

#include <lslboost/range/detail/msvc_has_iterator_workaround.hpp>

#endif
