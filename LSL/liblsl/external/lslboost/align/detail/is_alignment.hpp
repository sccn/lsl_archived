/*
(c) 2014 Glen Joseph Fernandes
glenjofe at gmail dot com

Distributed under the Boost Software
License, Version 1.0.
http://lslboost.org/LICENSE_1_0.txt
*/
#ifndef BOOST_ALIGN_DETAIL_IS_ALIGNMENT_HPP
#define BOOST_ALIGN_DETAIL_IS_ALIGNMENT_HPP

#include <lslboost/config.hpp>
#include <cstddef>

namespace lslboost {
namespace alignment {
namespace detail {

BOOST_CONSTEXPR inline bool is_alignment(std::size_t value)
    BOOST_NOEXCEPT
{
    return (value > 0) && ((value & (value - 1)) == 0);
}

} /* :detail */
} /* :alignment */
} /* :lslboost */

#endif
