//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_ROTATE_COPY_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_ROTATE_COPY_HPP_INCLUDED

#include <lslboost/concept_check.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/range/concepts.hpp>
#include <lslboost/range/iterator.hpp>
#include <algorithm>

namespace lslboost
{
    namespace range
    {

    /// \brief template function rotate
    ///
    /// range-based version of the rotate std algorithm
    ///
    /// \pre Rng meets the requirements for a Forward range
    template<typename ForwardRange, typename OutputIterator>
    inline OutputIterator rotate_copy(
        const ForwardRange&                                             rng,
        BOOST_DEDUCED_TYPENAME range_iterator<const ForwardRange>::type middle,
        OutputIterator                                                  target
        )
    {
        BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
        return std::rotate_copy(lslboost::begin(rng), middle, lslboost::end(rng), target);
    }

    } // namespace range
    using range::rotate_copy;
} // namespace lslboost

#endif // include guard
