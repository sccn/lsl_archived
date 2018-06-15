//  (C) Copyright John Maddock 2008.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_TR1_CMATH_HPP_INCLUDED
#  define BOOST_TR1_CMATH_HPP_INCLUDED
#  include <lslboost/tr1/detail/config.hpp>

#ifdef BOOST_HAS_TR1_CMATH

#  if defined(BOOST_HAS_INCLUDE_NEXT) && !defined(BOOST_TR1_DISABLE_INCLUDE_NEXT)
#     include_next BOOST_TR1_HEADER(cmath)
#  else
#     include <lslboost/tr1/detail/config_all.hpp>
#     include BOOST_TR1_HEADER(cmath)
#  endif

#else

#include <lslboost/math/tr1.hpp>

namespace std{ namespace tr1{

using lslboost::math::tr1::assoc_laguerre;
using lslboost::math::tr1::assoc_laguerref;
using lslboost::math::tr1::assoc_laguerrel;
// [5.2.1.2] associated Legendre functions:
using lslboost::math::tr1::assoc_legendre;
using lslboost::math::tr1::assoc_legendref;
using lslboost::math::tr1::assoc_legendrel;
// [5.2.1.3] beta function:
using lslboost::math::tr1::beta;
using lslboost::math::tr1::betaf;
using lslboost::math::tr1::betal;
// [5.2.1.4] (complete) elliptic integral of the first kind:
using lslboost::math::tr1::comp_ellint_1;
using lslboost::math::tr1::comp_ellint_1f;
using lslboost::math::tr1::comp_ellint_1l;
// [5.2.1.5] (complete) elliptic integral of the second kind:
using lslboost::math::tr1::comp_ellint_2;
using lslboost::math::tr1::comp_ellint_2f;
using lslboost::math::tr1::comp_ellint_2l;
// [5.2.1.6] (complete) elliptic integral of the third kind:
using lslboost::math::tr1::comp_ellint_3;
using lslboost::math::tr1::comp_ellint_3f;
using lslboost::math::tr1::comp_ellint_3l;
#if 0
// [5.2.1.7] confluent hypergeometric functions:
using lslboost::math::tr1::conf_hyperg;
using lslboost::math::tr1::conf_hypergf;
using lslboost::math::tr1::conf_hypergl;
#endif
// [5.2.1.8] regular modified cylindrical Bessel functions:
using lslboost::math::tr1::cyl_bessel_i;
using lslboost::math::tr1::cyl_bessel_if;
using lslboost::math::tr1::cyl_bessel_il;
// [5.2.1.9] cylindrical Bessel functions (of the first kind):
using lslboost::math::tr1::cyl_bessel_j;
using lslboost::math::tr1::cyl_bessel_jf;
using lslboost::math::tr1::cyl_bessel_jl;
// [5.2.1.10] irregular modified cylindrical Bessel functions:
using lslboost::math::tr1::cyl_bessel_k;
using lslboost::math::tr1::cyl_bessel_kf;
using lslboost::math::tr1::cyl_bessel_kl;
// [5.2.1.11] cylindrical Neumann functions;
// cylindrical Bessel functions (of the second kind):
using lslboost::math::tr1::cyl_neumann;
using lslboost::math::tr1::cyl_neumannf;
using lslboost::math::tr1::cyl_neumannl;
// [5.2.1.12] (incomplete) elliptic integral of the first kind:
using lslboost::math::tr1::ellint_1;
using lslboost::math::tr1::ellint_1f;
using lslboost::math::tr1::ellint_1l;
// [5.2.1.13] (incomplete) elliptic integral of the second kind:
using lslboost::math::tr1::ellint_2;
using lslboost::math::tr1::ellint_2f;
using lslboost::math::tr1::ellint_2l;
// [5.2.1.14] (incomplete) elliptic integral of the third kind:
using lslboost::math::tr1::ellint_3;
using lslboost::math::tr1::ellint_3f;
using lslboost::math::tr1::ellint_3l;
// [5.2.1.15] exponential integral:
using lslboost::math::tr1::expint;
using lslboost::math::tr1::expintf;
using lslboost::math::tr1::expintl;
// [5.2.1.16] Hermite polynomials:
using lslboost::math::tr1::hermite;
using lslboost::math::tr1::hermitef;
using lslboost::math::tr1::hermitel;
#if 0
// [5.2.1.17] hypergeometric functions:
using lslboost::math::tr1::hyperg;
using lslboost::math::tr1::hypergf;
using lslboost::math::tr1::hypergl;
#endif
// [5.2.1.18] Laguerre polynomials:
using lslboost::math::tr1::laguerre;
using lslboost::math::tr1::laguerref;
using lslboost::math::tr1::laguerrel;
// [5.2.1.19] Legendre polynomials:
using lslboost::math::tr1::legendre;
using lslboost::math::tr1::legendref;
using lslboost::math::tr1::legendrel;
// [5.2.1.20] Riemann zeta function:
using lslboost::math::tr1::riemann_zeta;
using lslboost::math::tr1::riemann_zetaf;
using lslboost::math::tr1::riemann_zetal;
// [5.2.1.21] spherical Bessel functions (of the first kind):
using lslboost::math::tr1::sph_bessel;
using lslboost::math::tr1::sph_besself;
using lslboost::math::tr1::sph_bessell;
// [5.2.1.22] spherical associated Legendre functions:
using lslboost::math::tr1::sph_legendre;
using lslboost::math::tr1::sph_legendref;
using lslboost::math::tr1::sph_legendrel;
// [5.2.1.23] spherical Neumann functions;
// spherical Bessel functions (of the second kind):
using lslboost::math::tr1::sph_neumann;
using lslboost::math::tr1::sph_neumannf;
using lslboost::math::tr1::sph_neumannl;

// types
using lslboost::math::tr1::double_t;
using lslboost::math::tr1::float_t;
// functions
using lslboost::math::tr1::acosh;
using lslboost::math::tr1::acoshf;
using lslboost::math::tr1::acoshl;
using lslboost::math::tr1::asinh;
using lslboost::math::tr1::asinhf;
using lslboost::math::tr1::asinhl;
using lslboost::math::tr1::atanh;
using lslboost::math::tr1::atanhf;
using lslboost::math::tr1::atanhl;
using lslboost::math::tr1::cbrt;
using lslboost::math::tr1::cbrtf;
using lslboost::math::tr1::cbrtl;
using lslboost::math::tr1::copysign;
using lslboost::math::tr1::copysignf;
using lslboost::math::tr1::copysignl;
using lslboost::math::tr1::erf;
using lslboost::math::tr1::erff;
using lslboost::math::tr1::erfl;
using lslboost::math::tr1::erfc;
using lslboost::math::tr1::erfcf;
using lslboost::math::tr1::erfcl;
#if 0
using lslboost::math::tr1::exp2;
using lslboost::math::tr1::exp2f;
using lslboost::math::tr1::exp2l;
#endif
using lslboost::math::tr1::expm1;
using lslboost::math::tr1::expm1f;
using lslboost::math::tr1::expm1l;
#if 0
using lslboost::math::tr1::fdim;
using lslboost::math::tr1::fdimf;
using lslboost::math::tr1::fdiml;
using lslboost::math::tr1::fma;
using lslboost::math::tr1::fmaf;
using lslboost::math::tr1::fmal;
#endif
using lslboost::math::tr1::fmax;
using lslboost::math::tr1::fmaxf;
using lslboost::math::tr1::fmaxl;
using lslboost::math::tr1::fmin;
using lslboost::math::tr1::fminf;
using lslboost::math::tr1::fminl;
using lslboost::math::tr1::hypot;
using lslboost::math::tr1::hypotf;
using lslboost::math::tr1::hypotl;
#if 0
using lslboost::math::tr1::ilogb;
using lslboost::math::tr1::ilogbf;
using lslboost::math::tr1::ilogbl;
#endif
using lslboost::math::tr1::lgamma;
using lslboost::math::tr1::lgammaf;
using lslboost::math::tr1::lgammal;
#if 0
using lslboost::math::tr1::llrint;
using lslboost::math::tr1::llrintf;
using lslboost::math::tr1::llrintl;
#endif
using lslboost::math::tr1::llround;
using lslboost::math::tr1::llroundf;
using lslboost::math::tr1::llroundl;
using lslboost::math::tr1::log1p;
using lslboost::math::tr1::log1pf;
using lslboost::math::tr1::log1pl;
#if 0
using lslboost::math::tr1::log2;
using lslboost::math::tr1::log2f;
using lslboost::math::tr1::log2l;
using lslboost::math::tr1::logb;
using lslboost::math::tr1::logbf;
using lslboost::math::tr1::logbl;
using lslboost::math::tr1::lrint;
using lslboost::math::tr1::lrintf;
using lslboost::math::tr1::lrintl;
#endif
using lslboost::math::tr1::lround;
using lslboost::math::tr1::lroundf;
using lslboost::math::tr1::lroundl;
#if 0
using lslboost::math::tr1::nan;
using lslboost::math::tr1::nanf;
using lslboost::math::tr1::nanl;
using lslboost::math::tr1::nearbyint;
using lslboost::math::tr1::nearbyintf;
using lslboost::math::tr1::nearbyintl;
#endif
using lslboost::math::tr1::nextafter;
using lslboost::math::tr1::nextafterf;
using lslboost::math::tr1::nextafterl;
using lslboost::math::tr1::nexttoward;
using lslboost::math::tr1::nexttowardf;
using lslboost::math::tr1::nexttowardl;
#if 0
using lslboost::math::tr1::remainder;
using lslboost::math::tr1::remainderf;
using lslboost::math::tr1::remainderl;
using lslboost::math::tr1::remquo;
using lslboost::math::tr1::remquof;
using lslboost::math::tr1::remquol;
using lslboost::math::tr1::rint;
using lslboost::math::tr1::rintf;
using lslboost::math::tr1::rintl;
#endif
using lslboost::math::tr1::round;
using lslboost::math::tr1::roundf;
using lslboost::math::tr1::roundl;
#if 0
using lslboost::math::tr1::scalbln;
using lslboost::math::tr1::scalblnf;
using lslboost::math::tr1::scalblnl;
using lslboost::math::tr1::scalbn;
using lslboost::math::tr1::scalbnf;
using lslboost::math::tr1::scalbnl;
#endif
using lslboost::math::tr1::tgamma;
using lslboost::math::tr1::tgammaf;
using lslboost::math::tr1::tgammal;
using lslboost::math::tr1::trunc;
using lslboost::math::tr1::truncf;
using lslboost::math::tr1::truncl;
// C99 macros defined as C++ templates
using lslboost::math::tr1::signbit;
using lslboost::math::tr1::fpclassify;
using lslboost::math::tr1::isfinite;
using lslboost::math::tr1::isinf;
using lslboost::math::tr1::isnan;
using lslboost::math::tr1::isnormal;
#if 0
using lslboost::math::tr1::isgreater;
using lslboost::math::tr1::isgreaterequal;
using lslboost::math::tr1::isless;
using lslboost::math::tr1::islessequal;
using lslboost::math::tr1::islessgreater;
using lslboost::math::tr1::isunordered;
#endif
} } // namespaces

#endif // BOOST_HAS_TR1_CMATH

#endif // BOOST_TR1_CMATH_HPP_INCLUDED
