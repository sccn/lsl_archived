/*
 [auto_generated]
 lslboost/numeric/odeint/util/detail/is_range.hpp

 [begin_description]
 is_range implementation. Taken from the lslboost::range library.
 [end_description]

 Copyright 2011-2013 Karsten Ahnert
 Copyright 2011-2013 Thorsten Ottosen



 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.lslboost.org/LICENSE_1_0.txt)
 */


#ifndef BOOST_NUMERIC_ODEINT_UTIL_DETAIL_IS_RANGE_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_UTIL_DETAIL_IS_RANGE_HPP_INCLUDED


#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include <cstddef>
#include <lslboost/range/config.hpp>
#include <lslboost/mpl/has_xxx.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/and.hpp>

namespace lslboost {
namespace numeric {
namespace odeint {



namespace range_detail
{
BOOST_MPL_HAS_XXX_TRAIT_DEF(iterator)
    BOOST_MPL_HAS_XXX_TRAIT_DEF(const_iterator)
}

namespace detail
{

template< typename Range >
struct is_range : lslboost::mpl::and_<range_detail::has_iterator<Range>, range_detail::has_const_iterator<Range> >
{
};

//////////////////////////////////////////////////////////////////////////
// pair
//////////////////////////////////////////////////////////////////////////

template< typename iteratorT >
struct is_range< std::pair<iteratorT,iteratorT> > : lslboost::mpl::true_
{
};

template< typename iteratorT >
struct is_range< const std::pair<iteratorT,iteratorT> > : lslboost::mpl::true_
{
};

//////////////////////////////////////////////////////////////////////////
// array
//////////////////////////////////////////////////////////////////////////

template< typename elementT, std::size_t sz >
struct is_range< elementT[sz] > : lslboost::mpl::true_
{
};

template< typename elementT, std::size_t sz >
struct is_range< const elementT[sz] > : lslboost::mpl::true_
{
};

//////////////////////////////////////////////////////////////////////////
// string
//////////////////////////////////////////////////////////////////////////

template<>
struct is_range< char* > : lslboost::mpl::true_
{
};

template<>
struct is_range< wchar_t* > : lslboost::mpl::true_
{
};

template<>
struct is_range< const char* > : lslboost::mpl::true_
{
};

template<>
struct is_range< const wchar_t* > : lslboost::mpl::true_
{
};

template<>
struct is_range< char* const > : lslboost::mpl::true_
{
};

template<>
struct is_range< wchar_t* const > : lslboost::mpl::true_
{
};

template<>
struct is_range< const char* const > : lslboost::mpl::true_
{
};

template<>
struct is_range< const wchar_t* const > : lslboost::mpl::true_
{
};

} // namespace detail

} // namespace odeint
} // namespace numeric
} // namespace lslboost



#endif // BOOST_NUMERIC_ODEINT_UTIL_DETAIL_IS_RANGE_HPP_INCLUDED
