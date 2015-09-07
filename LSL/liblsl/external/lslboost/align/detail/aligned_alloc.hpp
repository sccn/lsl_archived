/*
(c) 2014-2015 Glen Joseph Fernandes
glenjofe at gmail dot com

Distributed under the Boost Software
License, Version 1.0.
http://lslboost.org/LICENSE_1_0.txt
*/
#ifndef BOOST_ALIGN_DETAIL_ALIGNED_ALLOC_HPP
#define BOOST_ALIGN_DETAIL_ALIGNED_ALLOC_HPP

#include <lslboost/assert.hpp>
#include <lslboost/config.hpp>
#include <lslboost/align/align.hpp>
#include <lslboost/align/alignment_of.hpp>
#include <lslboost/align/detail/is_alignment.hpp>
#include <cstdlib>

namespace lslboost {
namespace alignment {

inline void* aligned_alloc(std::size_t alignment, std::size_t size)
    BOOST_NOEXCEPT
{
    BOOST_ASSERT(detail::is_alignment(alignment));
    enum {
        N = alignment_of<void*>::value
    };
    if (alignment < N) {
        alignment = N;
    }
    std::size_t n = size + alignment - N;
    void* p1 = 0;
    void* p2 = std::malloc(n + sizeof p1);
    if (p2) {
        p1 = static_cast<char*>(p2) + sizeof p1;
        (void)align(alignment, size, p1, n);
        *(static_cast<void**>(p1) - 1) = p2;
    }
    return p1;
}

inline void aligned_free(void* ptr) BOOST_NOEXCEPT
{
    if (ptr) {
        void* p = *(static_cast<void**>(ptr) - 1);
        std::free(p);
    }
}

} /* :alignment */
} /* :lslboost */

#endif
