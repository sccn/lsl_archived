// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_EXT_INSERT_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_EXT_INSERT_HPP_INCLUDED

#include <lslboost/range/config.hpp>
#include <lslboost/range/concepts.hpp>
#include <lslboost/range/difference_type.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/assert.hpp>

namespace lslboost
{
    namespace range
    {

template< class Container, class Range >
inline Container& insert( Container& on,
                          BOOST_DEDUCED_TYPENAME Container::iterator before,
                          const Range& from )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<Container> ));
    BOOST_RANGE_CONCEPT_ASSERT(( SinglePassRangeConcept<Range> ));
    BOOST_ASSERT( (void*)&on != (void*)&from &&
                  "cannot copy from a container to itself" );
    on.insert( before, lslboost::begin(from), lslboost::end(from) );
    return on;
}

    } // namespace range
    using range::insert;
} // namespace lslboost

#endif // include guard
