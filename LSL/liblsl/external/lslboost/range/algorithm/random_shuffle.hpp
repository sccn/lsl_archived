//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_RANDOM_SHUFFLE_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_RANDOM_SHUFFLE_HPP_INCLUDED

#include <lslboost/concept_check.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/range/concepts.hpp>
#include <algorithm>

namespace lslboost
{
    namespace range
    {

/// \brief template function random_shuffle
///
/// range-based version of the random_shuffle std algorithm
///
/// \pre RandomAccessRange is a model of the RandomAccessRangeConcept
/// \pre Generator is a model of the UnaryFunctionConcept
template<class RandomAccessRange>
inline RandomAccessRange& random_shuffle(RandomAccessRange& rng)
{
    BOOST_RANGE_CONCEPT_ASSERT(( RandomAccessRangeConcept<RandomAccessRange> ));
    std::random_shuffle(lslboost::begin(rng), lslboost::end(rng));
    return rng;
}

/// \overload
template<class RandomAccessRange>
inline const RandomAccessRange& random_shuffle(const RandomAccessRange& rng)
{
    BOOST_RANGE_CONCEPT_ASSERT(( RandomAccessRangeConcept<const RandomAccessRange> ));
    std::random_shuffle(lslboost::begin(rng), lslboost::end(rng));
    return rng;
}

/// \overload
template<class RandomAccessRange, class Generator>
inline RandomAccessRange& random_shuffle(RandomAccessRange& rng, Generator& gen)
{
    BOOST_RANGE_CONCEPT_ASSERT(( RandomAccessRangeConcept<RandomAccessRange> ));
    std::random_shuffle(lslboost::begin(rng), lslboost::end(rng), gen);
    return rng;
}

/// \overload
template<class RandomAccessRange, class Generator>
inline const RandomAccessRange& random_shuffle(const RandomAccessRange& rng, Generator& gen)
{
    BOOST_RANGE_CONCEPT_ASSERT(( RandomAccessRangeConcept<const RandomAccessRange> ));
    std::random_shuffle(lslboost::begin(rng), lslboost::end(rng), gen);
    return rng;
}

    } // namespace range
    using range::random_shuffle;
} // namespace lslboost

#endif // include guard
