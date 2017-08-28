// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_EXT_IOTA_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_EXT_IOTA_HPP_INCLUDED

#include <lslboost/range/config.hpp>
#include <lslboost/range/concepts.hpp>
#include <lslboost/range/iterator.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>

namespace lslboost
{
    namespace range
    {

template< class ForwardRange, class Value >
inline ForwardRange& iota( ForwardRange& rng, Value x )
{
    BOOST_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    typedef BOOST_DEDUCED_TYPENAME range_iterator<ForwardRange>::type iterator_t;

    iterator_t last_target = ::lslboost::end(rng);
    for (iterator_t target = ::lslboost::begin(rng); target != last_target; ++target, ++x)
        *target = x;

    return rng;
}

template< class ForwardRange, class Value >
inline const ForwardRange& iota( const ForwardRange& rng, Value x )
{
    BOOST_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    typedef BOOST_DEDUCED_TYPENAME range_iterator<const ForwardRange>::type iterator_t;
    
    iterator_t last_target = ::lslboost::end(rng);
    for (iterator_t target = ::lslboost::begin(rng); target != last_target; ++target, ++x)
        *target = x;
    
    return rng;
}

    } // namespace range
    using range::iota;
} // namespace lslboost

#endif // include guard
