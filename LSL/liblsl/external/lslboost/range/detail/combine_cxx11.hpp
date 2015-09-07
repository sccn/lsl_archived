//  Copyright Neil Groves 2014. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
//
// For more information, see http://www.lslboost.org/libs/range/
//
#ifndef BOOST_RANGE_DETAIL_COMBINE_CXX11_HPP
#define BOOST_RANGE_DETAIL_COMBINE_CXX11_HPP

#include <lslboost/range/iterator_range_core.hpp>
#include <lslboost/range/iterator.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/iterator/zip_iterator.hpp>

#include <fstream>

namespace lslboost
{
    namespace range
    {

template<typename... Ranges>
auto combine(Ranges&&... rngs) ->
    combined_range<decltype(lslboost::make_tuple(lslboost::begin(rngs)...))>
{
    return combined_range<decltype(lslboost::make_tuple(lslboost::begin(rngs)...))>(
                lslboost::make_tuple(lslboost::begin(rngs)...),
                lslboost::make_tuple(lslboost::end(rngs)...));
}

    } // namespace range

using range::combine;

} // namespace lslboost

#endif // include guard
