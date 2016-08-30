#ifndef BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
#define BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  bind/placeholders.hpp - _N definitions
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
//  See http://www.lslboost.org/libs/bind/bind.html for documentation.
//

#include <lslboost/bind/arg.hpp>
#include <lslboost/config.hpp>

namespace
{

#if defined(__BORLANDC__) || defined(__GNUC__) && (__GNUC__ < 4)

static inline lslboost::arg<1> _1() { return lslboost::arg<1>(); }
static inline lslboost::arg<2> _2() { return lslboost::arg<2>(); }
static inline lslboost::arg<3> _3() { return lslboost::arg<3>(); }
static inline lslboost::arg<4> _4() { return lslboost::arg<4>(); }
static inline lslboost::arg<5> _5() { return lslboost::arg<5>(); }
static inline lslboost::arg<6> _6() { return lslboost::arg<6>(); }
static inline lslboost::arg<7> _7() { return lslboost::arg<7>(); }
static inline lslboost::arg<8> _8() { return lslboost::arg<8>(); }
static inline lslboost::arg<9> _9() { return lslboost::arg<9>(); }

#elif defined(BOOST_MSVC) || (defined(__DECCXX_VER) && __DECCXX_VER <= 60590031) || defined(__MWERKS__) || \
    defined(__GNUC__) && (__GNUC__ == 4 && __GNUC_MINOR__ < 2)  

static lslboost::arg<1> _1;
static lslboost::arg<2> _2;
static lslboost::arg<3> _3;
static lslboost::arg<4> _4;
static lslboost::arg<5> _5;
static lslboost::arg<6> _6;
static lslboost::arg<7> _7;
static lslboost::arg<8> _8;
static lslboost::arg<9> _9;

#else

lslboost::arg<1> _1;
lslboost::arg<2> _2;
lslboost::arg<3> _3;
lslboost::arg<4> _4;
lslboost::arg<5> _5;
lslboost::arg<6> _6;
lslboost::arg<7> _7;
lslboost::arg<8> _8;
lslboost::arg<9> _9;

#endif

} // unnamed namespace

#endif // #ifndef BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
