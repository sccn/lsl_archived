// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_EXT_OVERWRITE_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_EXT_OVERWRITE_HPP_INCLUDED

#include <lslboost/range/config.hpp>
#include <lslboost/range/concepts.hpp>
#include <lslboost/range/difference_type.hpp>
#include <lslboost/range/iterator.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/assert.hpp>

namespace lslboost
{
    namespace range
    {

template< class SinglePassRange1, class SinglePassRange2 >
inline void overwrite( const SinglePassRange1& from, SinglePassRange2& to )
{
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange1> ));
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<SinglePassRange2> ));

    BOOST_DEDUCED_TYPENAME range_iterator<const SinglePassRange1>::type
        i = lslboost::begin(from), e = lslboost::end(from);

    BOOST_DEDUCED_TYPENAME range_iterator<SinglePassRange2>::type
        out = lslboost::begin(to);

#ifndef NDEBUG
    BOOST_DEDUCED_TYPENAME range_iterator<SinglePassRange2>::type
        last_out = lslboost::end(to);
#endif

    for( ; i != e; ++out, ++i )
    {
#ifndef NDEBUG
        BOOST_ASSERT( out != last_out
            && "out of bounds in lslboost::overwrite()" );
#endif
        *out = *i;
    }
}

template< class SinglePassRange1, class SinglePassRange2 >
inline void overwrite( const SinglePassRange1& from, const SinglePassRange2& to )
{
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange1> ));
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange2> ));

    BOOST_DEDUCED_TYPENAME range_iterator<const SinglePassRange1>::type
        i = lslboost::begin(from), e = lslboost::end(from);

    BOOST_DEDUCED_TYPENAME range_iterator<const SinglePassRange2>::type
        out = lslboost::begin(to);

#ifndef NDEBUG
    BOOST_DEDUCED_TYPENAME range_iterator<const SinglePassRange2>::type
        last_out = lslboost::end(to);
#endif

    for( ; i != e; ++out, ++i )
    {
#ifndef NDEBUG
        BOOST_ASSERT( out != last_out
            && "out of bounds in lslboost::overwrite()" );
#endif
        *out = *i;
    }
}

    } // namespace range
    using range::overwrite;
} // namespace lslboost

#endif // include guard
