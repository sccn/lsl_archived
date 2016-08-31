//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_REVERSE_COPY_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_REVERSE_COPY_HPP_INCLUDED

#include <lslboost/concept_check.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/range/concepts.hpp>
#include <lslboost/iterator/iterator_concepts.hpp>
#include <algorithm>

namespace lslboost
{
    namespace range
    {

/// \brief template function reverse_copy
///
/// range-based version of the reverse_copy std algorithm
///
/// \pre BidirectionalRange is a model of the BidirectionalRangeConcept
template<class BidirectionalRange, class OutputIterator>
inline OutputIterator reverse_copy(const BidirectionalRange& rng, OutputIterator out)
{
    BOOST_RANGE_CONCEPT_ASSERT(( BidirectionalRangeConcept<const BidirectionalRange> ));
    return std::reverse_copy(lslboost::begin(rng), lslboost::end(rng), out);
}

    } // namespace range
    using range::reverse_copy;
} // namespace lslboost

#endif // include guard
