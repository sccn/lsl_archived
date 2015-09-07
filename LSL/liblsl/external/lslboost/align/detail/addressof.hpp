/*
(c) 2014 Glen Joseph Fernandes
glenjofe at gmail dot com

Distributed under the Boost Software
License, Version 1.0.
http://lslboost.org/LICENSE_1_0.txt
*/
#ifndef BOOST_ALIGN_DETAIL_ADDRESSOF_HPP
#define BOOST_ALIGN_DETAIL_ADDRESSOF_HPP

#include <lslboost/config.hpp>

#if !defined(BOOST_NO_CXX11_ADDRESSOF)
#include <memory>
#else
#include <lslboost/core/addressof.hpp>
#endif

namespace lslboost {
namespace alignment {
namespace detail {

#if !defined(BOOST_NO_CXX11_ADDRESSOF)
using std::addressof;
#else
using lslboost::addressof;
#endif

} /* :detail */
} /* :alignment */
} /* :lslboost */

#endif
