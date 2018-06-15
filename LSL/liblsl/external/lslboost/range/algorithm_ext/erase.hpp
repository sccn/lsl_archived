// Boost.Range library
//
//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_EXT_ERASE_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_EXT_ERASE_HPP_INCLUDED

#include <lslboost/range/config.hpp>
#include <lslboost/range/concepts.hpp>
#include <lslboost/range/difference_type.hpp>
#include <lslboost/range/iterator_range_core.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/assert.hpp>

namespace lslboost
{
    namespace range
    {

template< class Container >
inline Container& erase( Container& on,
      iterator_range<BOOST_DEDUCED_TYPENAME Container::iterator> to_erase )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<Container> ));
    on.erase( lslboost::begin(to_erase), lslboost::end(to_erase) );
    return on;
}

template< class Container, class T >
inline Container& remove_erase( Container& on, const T& val )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<Container> ));
    on.erase(
        std::remove(lslboost::begin(on), lslboost::end(on), val),
        lslboost::end(on));
    return on;
}

template< class Container, class Pred >
inline Container& remove_erase_if( Container& on, Pred pred )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<Container> ));
    on.erase(
        std::remove_if(lslboost::begin(on), lslboost::end(on), pred),
        lslboost::end(on));
    return on;
}

    } // namespace range
    using range::erase;
    using range::remove_erase;
    using range::remove_erase_if;
} // namespace lslboost

#endif // include guard
