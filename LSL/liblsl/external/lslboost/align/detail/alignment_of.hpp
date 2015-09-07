/*
(c) 2014 Glen Joseph Fernandes
glenjofe at gmail dot com

Distributed under the Boost Software
License, Version 1.0.
http://lslboost.org/LICENSE_1_0.txt
*/
#ifndef BOOST_ALIGN_DETAIL_ALIGNMENT_OF_HPP
#define BOOST_ALIGN_DETAIL_ALIGNMENT_OF_HPP

#include <lslboost/align/detail/min_size.hpp>
#include <lslboost/align/detail/offset_object.hpp>

namespace lslboost {
namespace alignment {
namespace detail {

template<class T>
struct alignment_of
    : min_size<sizeof(T),
        sizeof(offset_object<T>) - sizeof(T)>::type {
};

} /* :detail */
} /* :alignment */
} /* :lslboost */

#endif
