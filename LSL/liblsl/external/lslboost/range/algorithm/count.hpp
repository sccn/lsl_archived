//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_COUNT_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_COUNT_HPP_INCLUDED

#include <lslboost/concept_check.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/range/concepts.hpp>
#include <lslboost/range/difference_type.hpp>
#include <algorithm>

namespace lslboost
{
    namespace range
    {

/// \brief template function count
///
/// range-based version of the count std algorithm
///
/// \pre SinglePassRange is a model of the SinglePassRangeConcept
template< class SinglePassRange, class Value >
inline BOOST_DEDUCED_TYPENAME range_difference<SinglePassRange>::type
count(SinglePassRange& rng, const Value& val)
{
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<SinglePassRange> ));
    return std::count(lslboost::begin(rng), lslboost::end(rng), val);
}

/// \overload
template< class SinglePassRange, class Value >
inline BOOST_DEDUCED_TYPENAME range_difference<SinglePassRange const>::type
count(const SinglePassRange& rng, const Value& val)
{
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange> ));
    return std::count(lslboost::begin(rng), lslboost::end(rng), val);
}

    } // namespace range
    using range::count;
} // namespace lslboost

#endif // include guard
