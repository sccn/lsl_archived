//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_UNIQUE_COPY_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_UNIQUE_COPY_HPP_INCLUDED

#include <lslboost/concept_check.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/range/concepts.hpp>
#include <algorithm>

namespace lslboost
{
    namespace range
    {

/// \brief template function unique_copy
///
/// range-based version of the unique_copy std algorithm
///
/// \pre SinglePassRange is a model of the SinglePassRangeConcept
/// \pre OutputIterator is a model of the OutputIteratorConcept
/// \pre BinaryPredicate is a model of the BinaryPredicateConcept
template< class SinglePassRange, class OutputIterator >
inline OutputIterator
unique_copy( const SinglePassRange& rng, OutputIterator out_it )
{
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange> ));
    return std::unique_copy(lslboost::begin(rng), lslboost::end(rng), out_it);
}
/// \overload
template< class SinglePassRange, class OutputIterator, class BinaryPredicate >
inline OutputIterator
unique_copy( const SinglePassRange& rng, OutputIterator out_it,
             BinaryPredicate pred )
{
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange> ));
    return std::unique_copy(lslboost::begin(rng), lslboost::end(rng), out_it, pred);
}

    } // namespace range
    using range::unique_copy;
} // namespace lslboost

#endif // include guard
