//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_PARTITION__HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_PARTITION__HPP_INCLUDED

#include <lslboost/concept_check.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/range/concepts.hpp>
#include <lslboost/range/detail/range_return.hpp>
#include <algorithm>

namespace lslboost
{
    namespace range
    {

/// \brief template function partition
///
/// range-based version of the partition std algorithm
///
/// \pre ForwardRange is a model of the ForwardRangeConcept
template<class ForwardRange, class UnaryPredicate>
inline BOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type
partition(ForwardRange& rng, UnaryPredicate pred)
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return std::partition(lslboost::begin(rng),lslboost::end(rng),pred);
}

/// \overload
template<class ForwardRange, class UnaryPredicate>
inline BOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type
partition(const ForwardRange& rng, UnaryPredicate pred)
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return std::partition(lslboost::begin(rng),lslboost::end(rng),pred);
}

// range_return overloads

/// \overload
template< range_return_value re, class ForwardRange,
          class UnaryPredicate >
inline BOOST_DEDUCED_TYPENAME range_return<ForwardRange,re>::type
partition(ForwardRange& rng, UnaryPredicate pred)
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return lslboost::range_return<ForwardRange,re>::
        pack(std::partition(lslboost::begin(rng), lslboost::end(rng), pred), rng);
}

/// \overload
template< range_return_value re, class ForwardRange,
          class UnaryPredicate >
inline BOOST_DEDUCED_TYPENAME range_return<const ForwardRange,re>::type
partition(const ForwardRange& rng, UnaryPredicate pred)
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return lslboost::range_return<const ForwardRange,re>::
        pack(std::partition(lslboost::begin(rng), lslboost::end(rng), pred), rng);
}

    } // namespace range
    using range::partition;
} // namespace lslboost

#endif // include guard
