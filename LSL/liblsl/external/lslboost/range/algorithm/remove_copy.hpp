//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_REMOVE_COPY_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_REMOVE_COPY_HPP_INCLUDED

#include <lslboost/concept_check.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/range/concepts.hpp>
#include <algorithm>

namespace lslboost
{
    namespace range
    {

/// \brief template function remove_copy
///
/// range-based version of the remove_copy std algorithm
///
/// \pre SinglePassRange is a model of the SinglePassRangeConcept
/// \pre OutputIterator is a model of the OutputIteratorConcept
/// \pre Value is a model of the EqualityComparableConcept
/// \pre Objects of type Value can be compared for equality with objects of
/// InputIterator's value type.
template< class SinglePassRange, class OutputIterator, class Value >
inline OutputIterator
remove_copy(const SinglePassRange& rng, OutputIterator out_it, const Value& val)
{
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange> ));
    return std::remove_copy(lslboost::begin(rng), lslboost::end(rng), out_it, val);
}

    } // namespace range
    using range::remove_copy;
} // namespace lslboost

#endif // include guard
