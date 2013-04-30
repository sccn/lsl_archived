//  Copyright Neil Groves 2009. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_ALGORITHM_UNIQUE_HPP_INCLUDED
#define BOOST_RANGE_ALGORITHM_UNIQUE_HPP_INCLUDED

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

/// \brief template function unique
///
/// range-based version of the unique std algorithm
///
/// \pre Rng meets the requirements for a Forward range
template< range_return_value re, class ForwardRange >
inline BOOST_DEDUCED_TYPENAME range_return<ForwardRange,re>::type
unique( ForwardRange& rng )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return range_return<ForwardRange,re>::
        pack( std::unique( lslboost::begin(rng),
                           lslboost::end(rng)), rng );
}

/// \overload
template< range_return_value re, class ForwardRange >
inline BOOST_DEDUCED_TYPENAME range_return<const ForwardRange,re>::type
unique( const ForwardRange& rng )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return range_return<const ForwardRange,re>::
        pack( std::unique( lslboost::begin(rng),
                           lslboost::end(rng)), rng );
}
/// \overload
template< range_return_value re, class ForwardRange, class BinaryPredicate >
inline BOOST_DEDUCED_TYPENAME range_return<ForwardRange,re>::type
unique( ForwardRange& rng, BinaryPredicate pred )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return range_return<ForwardRange,re>::
        pack(std::unique(lslboost::begin(rng), lslboost::end(rng), pred),
             rng);
}
/// \overload
template< range_return_value re, class ForwardRange, class BinaryPredicate >
inline BOOST_DEDUCED_TYPENAME range_return<const ForwardRange,re>::type
unique( const ForwardRange& rng, BinaryPredicate pred )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return range_return<const ForwardRange,re>::
        pack(std::unique(lslboost::begin(rng), lslboost::end(rng), pred),
             rng);
}

/// \overload
template< class ForwardRange >
inline BOOST_DEDUCED_TYPENAME range_return<ForwardRange, return_begin_found>::type
unique( ForwardRange& rng )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return ::lslboost::range::unique<return_begin_found>(rng);
}
/// \overload
template< class ForwardRange >
inline BOOST_DEDUCED_TYPENAME range_return<const ForwardRange, return_begin_found>::type
unique( const ForwardRange& rng )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return ::lslboost::range::unique<return_begin_found>(rng);
}
/// \overload
template< class ForwardRange, class BinaryPredicate >
inline BOOST_DEDUCED_TYPENAME range_return<ForwardRange, return_begin_found>::type
unique( ForwardRange& rng, BinaryPredicate pred )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<ForwardRange> ));
    return ::lslboost::range::unique<return_begin_found>(rng);
}
/// \overload
template< class ForwardRange, class BinaryPredicate >
inline BOOST_DEDUCED_TYPENAME range_iterator<const ForwardRange>::type
unique( const ForwardRange& rng, BinaryPredicate pred )
{
    BOOST_RANGE_CONCEPT_ASSERT(( ForwardRangeConcept<const ForwardRange> ));
    return ::lslboost::range::unique<return_begin_found>(rng, pred);
}

    } // namespace range
    using range::unique;
} // namespace lslboost

#endif // include guard
