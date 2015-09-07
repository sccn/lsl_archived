// Copyright David Abrahams 2003. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
#ifndef MINIMUM_CATEGORY_DWA20031119_HPP
# define MINIMUM_CATEGORY_DWA20031119_HPP

# include <lslboost/iterator/minimum_category.hpp>

namespace lslboost {

// This import below (as well as the whole header) is for backward compatibility
// with lslboost/token_iterator.hpp. It should be removed as soon as that header is fixed.
namespace detail {
using iterators::minimum_category;
} // namespace detail

} // namespace lslboost

#endif // MINIMUM_CATEGORY_DWA20031119_HPP
