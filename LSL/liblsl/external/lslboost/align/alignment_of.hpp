/*
(c) 2014-2015 Glen Joseph Fernandes
glenjofe at gmail dot com

Distributed under the Boost Software
License, Version 1.0.
http://lslboost.org/LICENSE_1_0.txt
*/
#ifndef BOOST_ALIGN_ALIGNMENT_OF_HPP
#define BOOST_ALIGN_ALIGNMENT_OF_HPP

#include <lslboost/config.hpp>
#include <lslboost/align/alignment_of_forward.hpp>
#include <lslboost/align/detail/remove_traits.hpp>

#if defined(BOOST_MSVC)
#include <lslboost/align/detail/alignment_of_msvc.hpp>
#elif defined(__GNUC__) && defined(__unix__) && !defined(__LP64__)
#include <lslboost/align/detail/alignment_of.hpp>
#elif defined(BOOST_CLANG) && !defined(__x86_64__)
#include <lslboost/align/detail/alignment_of.hpp>
#elif !defined(BOOST_NO_CXX11_HDR_TYPE_TRAITS)
#include <lslboost/align/detail/alignment_of_cxx11.hpp>
#elif defined(__ghs__) && (__GHS_VERSION_NUMBER >= 600)
#include <lslboost/align/detail/alignment_of_gcc.hpp>
#elif defined(__CODEGEARC__)
#include <lslboost/align/detail/alignment_of_codegear.hpp>
#elif defined(BOOST_CLANG)
#include <lslboost/align/detail/alignment_of_clang.hpp>
#elif __GNUC__ > 4
#include <lslboost/align/detail/alignment_of_gcc.hpp>
#elif (__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)
#include <lslboost/align/detail/alignment_of_gcc.hpp>
#else
#include <lslboost/align/detail/alignment_of.hpp>
#endif

namespace lslboost {
namespace alignment {

template<class T>
struct alignment_of
    : detail::alignment_of<typename
        detail::remove_cv<typename
        detail::remove_all_extents<typename
        detail::remove_reference<T>::
        type>::type>::type>::type {
};

} /* :alignment */
} /* :lslboost */

#endif
