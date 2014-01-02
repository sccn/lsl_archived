//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_PARTIAL_SORT_COPY_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_PARTIAL_SORT_COPY_HPP_INCLUDED

#include <lslboost/concept_check.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/range/concepts.hpp>
#include <lslboost/range/value_type.hpp>
#include <algorithm>

namespace lslboost
{
    namespace range
    {

/// \brief template function partial_sort_copy
///
/// range-based version of the partial_sort_copy std algorithm
///
/// \pre SinglePassRange is a model of the SinglePassRangeConcept
/// \pre RandomAccessRange is a model of the Mutable_RandomAccessRangeConcept
/// \pre BinaryPredicate is a model of the BinaryPredicateConcept
template<class SinglePassRange, class RandomAccessRange>
inline BOOST_DEDUCED_TYPENAME range_iterator<RandomAccessRange>::type
partial_sort_copy(const SinglePassRange& rng1, RandomAccessRange& rng2)
{
    BOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<const SinglePassRange>));

    return std::partial_sort_copy(lslboost::begin(rng1), lslboost::end(rng1),
        lslboost::begin(rng2), lslboost::end(rng2));
}

/// \overload
template<class SinglePassRange, class RandomAccessRange>
inline BOOST_DEDUCED_TYPENAME range_iterator<RandomAccessRange>::type
partial_sort_copy(const SinglePassRange& rng1, const RandomAccessRange& rng2)
{
    BOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<const SinglePassRange>));

    return std::partial_sort_copy(lslboost::begin(rng1), lslboost::end(rng1),
        lslboost::begin(rng2), lslboost::end(rng2));
}

/// \overload
template<class SinglePassRange, class RandomAccessRange,
         class BinaryPredicate>
inline BOOST_DEDUCED_TYPENAME range_iterator<RandomAccessRange>::type
partial_sort_copy(const SinglePassRange& rng1, RandomAccessRange& rng2,
    BinaryPredicate pred)
{
    BOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<const SinglePassRange>));

    return std::partial_sort_copy(lslboost::begin(rng1), lslboost::end(rng1),
        lslboost::begin(rng2), lslboost::end(rng2), pred);
}

/// \overload
template<class SinglePassRange, class RandomAccessRange,
         class BinaryPredicate>
inline BOOST_DEDUCED_TYPENAME range_iterator<const RandomAccessRange>::type
partial_sort_copy(const SinglePassRange& rng1, const RandomAccessRange& rng2,
    BinaryPredicate pred)
{
    BOOST_RANGE_CONCEPT_ASSERT((SinglePassRangeConcept<const SinglePassRange>));

    return std::partial_sort_copy(lslboost::begin(rng1), lslboost::end(rng1),
        lslboost::begin(rng2), lslboost::end(rng2), pred);
}

    } // namespace range
    using range::partial_sort_copy;
} // namespace lslboost

#endif // include guard
