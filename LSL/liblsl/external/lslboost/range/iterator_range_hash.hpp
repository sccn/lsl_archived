// Boost.Range library
//
//  Copyright Neil Groves 2014
//  Use, modification and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//
#include <lslboost/range/iterator_range_core.hpp>
#include <lslboost/functional/hash.hpp>

namespace lslboost
{

template<class T>
std::size_t hash_value(const iterator_range<T>& rng)
{
    return lslboost::hash_range(rng.begin(), rng.end());
}

} // namespace lslboost
