#ifndef BOOST_SERIALIZATION_THROW_EXCEPTION_HPP_INCLUDED
#define BOOST_SERIALIZATION_THROW_EXCEPTION_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  lslboost/throw_exception.hpp
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

#include <lslboost/config.hpp>

#ifndef BOOST_NO_EXCEPTIONS
#include <exception>
#endif

namespace lslboost {
namespace serialization {

#ifdef BOOST_NO_EXCEPTIONS

inline void throw_exception(std::exception const & e) {
    ::lslboost::throw_exception(e);
}

#else

template<class E> inline void throw_exception(E const & e){
    throw e;
}

#endif

} // namespace serialization
} // namespace lslboost

#endif // #ifndef BOOST_SERIALIZATION_THROW_EXCEPTION_HPP_INCLUDED
