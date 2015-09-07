/*
(c) 2014 Glen Joseph Fernandes
glenjofe at gmail dot com

Distributed under the Boost Software
License, Version 1.0.
http://lslboost.org/LICENSE_1_0.txt
*/
#ifndef BOOST_ALIGN_DETAIL_ALIGNMENT_OF_CODEGEAR_HPP
#define BOOST_ALIGN_DETAIL_ALIGNMENT_OF_CODEGEAR_HPP

#include <lslboost/align/detail/integral_constant.hpp>
#include <cstddef>

namespace lslboost {
namespace alignment {
namespace detail {

template<class T>
struct alignment_of
    : integral_constant<std::size_t, alignof(T)> {
};

} /* :detail */
} /* :alignment */
} /* :lslboost */

#endif
