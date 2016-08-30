//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_TRANSFORM_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_TRANSFORM_HPP_INCLUDED

#include <lslboost/assert.hpp>
#include <lslboost/concept_check.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/range/concepts.hpp>
#include <algorithm>

namespace lslboost
{
    namespace range
    {

        /// \brief template function transform
        ///
        /// range-based version of the transform std algorithm
        ///
        /// \pre SinglePassRange1 is a model of the SinglePassRangeConcept
        /// \pre SinglePassRange2 is a model of the SinglePassRangeConcept
        /// \pre OutputIterator is a model of the OutputIteratorConcept
        /// \pre UnaryOperation is a model of the UnaryFunctionConcept
        /// \pre BinaryOperation is a model of the BinaryFunctionConcept
        template< class SinglePassRange1,
                  class OutputIterator,
                  class UnaryOperation >
        inline OutputIterator
        transform(const SinglePassRange1& rng,
                  OutputIterator          out,
                  UnaryOperation          fun)
        {
            BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange1> ));
            return std::transform(lslboost::begin(rng),lslboost::end(rng),out,fun);
        }

    } // namespace range

    namespace range_detail
    {
        template< class SinglePassTraversalReadableIterator1,
                  class SinglePassTraversalReadableIterator2,
                  class OutputIterator,
                  class BinaryFunction >
        inline OutputIterator
        transform_impl(SinglePassTraversalReadableIterator1 first1,
                       SinglePassTraversalReadableIterator1 last1,
                       SinglePassTraversalReadableIterator2 first2,
                       SinglePassTraversalReadableIterator2 last2,
                       OutputIterator                       out,
                       BinaryFunction                       fn)
        {
            for (; first1 != last1; ++first1, ++first2)
            {
                BOOST_ASSERT( first2 != last2 );
                *out = fn(*first1, *first2);
                ++out;
            }
            return out;
        }
    }

    namespace range
    {

        /// \overload
        template< class SinglePassRange1,
                  class SinglePassRange2,
                  class OutputIterator,
                  class BinaryOperation >
        inline OutputIterator
        transform(const SinglePassRange1& rng1,
                  const SinglePassRange2& rng2,
                  OutputIterator          out,
                  BinaryOperation         fun)
        {
            BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange1> ));
            BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<const SinglePassRange2> ));
            return lslboost::range_detail::transform_impl(
                        lslboost::begin(rng1), lslboost::end(rng1),
                        lslboost::begin(rng2), lslboost::end(rng2),
                        out, fun);
        }

    } // namespace range
    using range::transform;
} // namespace lslboost

#endif // include guard
