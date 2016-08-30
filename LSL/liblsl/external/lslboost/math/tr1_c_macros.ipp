// Copyright John Maddock 2008-11.
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_C_MACROS_IPP
#define BOOST_MATH_C_MACROS_IPP

// C99 Functions:
#ifdef acosh
#undef acosh
#endif
#define acosh lslboost_acosh
#ifdef acoshf
#undef acoshf
#endif
#define acoshf lslboost_acoshf
#ifdef acoshl
#undef acoshl
#endif
#define acoshl lslboost_acoshl

#ifdef asinh
#undef asinh
#endif
#define asinh lslboost_asinh
#ifdef asinhf
#undef asinhf
#endif
#define asinhf lslboost_asinhf
#ifdef asinhl
#undef asinhl
#endif
#define asinhl lslboost_asinhl

#ifdef atanh
#undef atanh
#endif
#define atanh lslboost_atanh
#ifdef atanhf
#undef atanhf
#endif
#define atanhf lslboost_atanhf
#ifdef atanhl
#undef atanhl
#endif
#define atanhl lslboost_atanhl

#ifdef cbrt
#undef cbrt
#endif
#define cbrt lslboost_cbrt
#ifdef cbrtf
#undef cbrtf
#endif
#define cbrtf lslboost_cbrtf
#ifdef cbrtl
#undef cbrtl
#endif
#define cbrtl lslboost_cbrtl

#ifdef copysign
#undef copysign
#endif
#define copysign lslboost_copysign
#ifdef copysignf
#undef copysignf
#endif
#define copysignf lslboost_copysignf
#ifdef copysignl
#undef copysignl
#endif
#define copysignl lslboost_copysignl

#ifdef erf
#undef erf
#endif
#define erf lslboost_erf
#ifdef erff
#undef erff
#endif
#define erff lslboost_erff
#ifdef erfl
#undef erfl
#endif
#define erfl lslboost_erfl

#ifdef erfc
#undef erfc
#endif
#define erfc lslboost_erfc
#ifdef erfcf
#undef erfcf
#endif
#define erfcf lslboost_erfcf
#ifdef erfcl
#undef erfcl
#endif
#define erfcl lslboost_erfcl

#if 0
#ifdef exp2
#undef exp2
#endif
#define exp2 lslboost_exp2
#ifdef exp2f
#undef exp2f
#endif
#define exp2f lslboost_exp2f
#ifdef exp2l
#undef exp2l
#endif
#define exp2l lslboost_exp2l
#endif

#ifdef expm1
#undef expm1
#endif
#define expm1 lslboost_expm1
#ifdef expm1f
#undef expm1f
#endif
#define expm1f lslboost_expm1f
#ifdef expm1l
#undef expm1l
#endif
#define expm1l lslboost_expm1l

#if 0
#ifdef fdim
#undef fdim
#endif
#define fdim lslboost_fdim
#ifdef fdimf
#undef fdimf
#endif
#define fdimf lslboost_fdimf
#ifdef fdiml
#undef fdiml
#endif
#define fdiml lslboost_fdiml
#ifdef acosh
#undef acosh
#endif
#define fma lslboost_fma
#ifdef fmaf
#undef fmaf
#endif
#define fmaf lslboost_fmaf
#ifdef fmal
#undef fmal
#endif
#define fmal lslboost_fmal
#endif

#ifdef fmax
#undef fmax
#endif
#define fmax lslboost_fmax
#ifdef fmaxf
#undef fmaxf
#endif
#define fmaxf lslboost_fmaxf
#ifdef fmaxl
#undef fmaxl
#endif
#define fmaxl lslboost_fmaxl

#ifdef fmin
#undef fmin
#endif
#define fmin lslboost_fmin
#ifdef fminf
#undef fminf
#endif
#define fminf lslboost_fminf
#ifdef fminl
#undef fminl
#endif
#define fminl lslboost_fminl

#ifdef hypot
#undef hypot
#endif
#define hypot lslboost_hypot
#ifdef hypotf
#undef hypotf
#endif
#define hypotf lslboost_hypotf
#ifdef hypotl
#undef hypotl
#endif
#define hypotl lslboost_hypotl

#if 0
#ifdef ilogb
#undef ilogb
#endif
#define ilogb lslboost_ilogb
#ifdef ilogbf
#undef ilogbf
#endif
#define ilogbf lslboost_ilogbf
#ifdef ilogbl
#undef ilogbl
#endif
#define ilogbl lslboost_ilogbl
#endif

#ifdef lgamma
#undef lgamma
#endif
#define lgamma lslboost_lgamma
#ifdef lgammaf
#undef lgammaf
#endif
#define lgammaf lslboost_lgammaf
#ifdef lgammal
#undef lgammal
#endif
#define lgammal lslboost_lgammal

#ifdef BOOST_HAS_LONG_LONG
#if 0
#ifdef llrint
#undef llrint
#endif
#define llrint lslboost_llrint
#ifdef llrintf
#undef llrintf
#endif
#define llrintf lslboost_llrintf
#ifdef llrintl
#undef llrintl
#endif
#define llrintl lslboost_llrintl
#endif
#ifdef llround
#undef llround
#endif
#define llround lslboost_llround
#ifdef llroundf
#undef llroundf
#endif
#define llroundf lslboost_llroundf
#ifdef llroundl
#undef llroundl
#endif
#define llroundl lslboost_llroundl
#endif

#ifdef log1p
#undef log1p
#endif
#define log1p lslboost_log1p
#ifdef log1pf
#undef log1pf
#endif
#define log1pf lslboost_log1pf
#ifdef log1pl
#undef log1pl
#endif
#define log1pl lslboost_log1pl

#if 0
#ifdef log2
#undef log2
#endif
#define log2 lslboost_log2
#ifdef log2f
#undef log2f
#endif
#define log2f lslboost_log2f
#ifdef log2l
#undef log2l
#endif
#define log2l lslboost_log2l

#ifdef logb
#undef logb
#endif
#define logb lslboost_logb
#ifdef logbf
#undef logbf
#endif
#define logbf lslboost_logbf
#ifdef logbl
#undef logbl
#endif
#define logbl lslboost_logbl

#ifdef lrint
#undef lrint
#endif
#define lrint lslboost_lrint
#ifdef lrintf
#undef lrintf
#endif
#define lrintf lslboost_lrintf
#ifdef lrintl
#undef lrintl
#endif
#define lrintl lslboost_lrintl
#endif

#ifdef lround
#undef lround
#endif
#define lround lslboost_lround
#ifdef lroundf
#undef lroundf
#endif
#define lroundf lslboost_lroundf
#ifdef lroundl
#undef lroundl
#endif
#define lroundl lslboost_lroundl

#if 0
#ifdef nan
#undef nan
#endif
#define nan lslboost_nan
#ifdef nanf
#undef nanf
#endif
#define nanf lslboost_nanf
#ifdef nanl
#undef nanl
#endif
#define nanl lslboost_nanl

#ifdef nearbyint
#undef nearbyint
#endif
#define nearbyint lslboost_nearbyint
#ifdef nearbyintf
#undef nearbyintf
#endif
#define nearbyintf lslboost_nearbyintf
#ifdef nearbyintl
#undef nearbyintl
#endif
#define nearbyintl lslboost_nearbyintl
#endif

#ifdef nextafter
#undef nextafter
#endif
#define nextafter lslboost_nextafter
#ifdef nextafterf
#undef nextafterf
#endif
#define nextafterf lslboost_nextafterf
#ifdef nextafterl
#undef nextafterl
#endif
#define nextafterl lslboost_nextafterl

#ifdef nexttoward
#undef nexttoward
#endif
#define nexttoward lslboost_nexttoward
#ifdef nexttowardf
#undef nexttowardf
#endif
#define nexttowardf lslboost_nexttowardf
#ifdef nexttowardl
#undef nexttowardl
#endif
#define nexttowardl lslboost_nexttowardl

#if 0
#ifdef remainder
#undef remainder
#endif
#define remainder lslboost_remainder
#ifdef remainderf
#undef remainderf
#endif
#define remainderf lslboost_remainderf
#ifdef remainderl
#undef remainderl
#endif
#define remainderl lslboost_remainderl

#ifdef remquo
#undef remquo
#endif
#define remquo lslboost_remquo
#ifdef remquof
#undef remquof
#endif
#define remquof lslboost_remquof
#ifdef remquol
#undef remquol
#endif
#define remquol lslboost_remquol

#ifdef rint
#undef rint
#endif
#define rint lslboost_rint
#ifdef rintf
#undef rintf
#endif
#define rintf lslboost_rintf
#ifdef rintl
#undef rintl
#endif
#define rintl lslboost_rintl
#endif

#ifdef round
#undef round
#endif
#define round lslboost_round
#ifdef roundf
#undef roundf
#endif
#define roundf lslboost_roundf
#ifdef roundl
#undef roundl
#endif
#define roundl lslboost_roundl

#if 0
#ifdef scalbln
#undef scalbln
#endif
#define scalbln lslboost_scalbln
#ifdef scalblnf
#undef scalblnf
#endif
#define scalblnf lslboost_scalblnf
#ifdef scalblnl
#undef scalblnl
#endif
#define scalblnl lslboost_scalblnl

#ifdef scalbn
#undef scalbn
#endif
#define scalbn lslboost_scalbn
#ifdef scalbnf
#undef scalbnf
#endif
#define scalbnf lslboost_scalbnf
#ifdef scalbnl
#undef scalbnl
#endif
#define scalbnl lslboost_scalbnl
#endif

#ifdef tgamma
#undef tgamma
#endif
#define tgamma lslboost_tgamma
#ifdef tgammaf
#undef tgammaf
#endif
#define tgammaf lslboost_tgammaf
#ifdef tgammal
#undef tgammal
#endif
#define tgammal lslboost_tgammal

#ifdef trunc
#undef trunc
#endif
#define trunc lslboost_trunc
#ifdef truncf
#undef truncf
#endif
#define truncf lslboost_truncf
#ifdef truncl
#undef truncl
#endif
#define truncl lslboost_truncl

// [5.2.1.1] associated Laguerre polynomials:
#ifdef assoc_laguerre
#undef assoc_laguerre
#endif
#define assoc_laguerre lslboost_assoc_laguerre
#ifdef assoc_laguerref
#undef assoc_laguerref
#endif
#define assoc_laguerref lslboost_assoc_laguerref
#ifdef assoc_laguerrel
#undef assoc_laguerrel
#endif
#define assoc_laguerrel lslboost_assoc_laguerrel

// [5.2.1.2] associated Legendre functions:
#ifdef assoc_legendre
#undef assoc_legendre
#endif
#define assoc_legendre lslboost_assoc_legendre
#ifdef assoc_legendref
#undef assoc_legendref
#endif
#define assoc_legendref lslboost_assoc_legendref
#ifdef assoc_legendrel
#undef assoc_legendrel
#endif
#define assoc_legendrel lslboost_assoc_legendrel

// [5.2.1.3] beta function:
#ifdef beta
#undef beta
#endif
#define beta lslboost_beta
#ifdef betaf
#undef betaf
#endif
#define betaf lslboost_betaf
#ifdef betal
#undef betal
#endif
#define betal lslboost_betal

// [5.2.1.4] (complete) elliptic integral of the first kind:
#ifdef comp_ellint_1
#undef comp_ellint_1
#endif
#define comp_ellint_1 lslboost_comp_ellint_1
#ifdef comp_ellint_1f
#undef comp_ellint_1f
#endif
#define comp_ellint_1f lslboost_comp_ellint_1f
#ifdef comp_ellint_1l
#undef comp_ellint_1l
#endif
#define comp_ellint_1l lslboost_comp_ellint_1l

// [5.2.1.5] (complete) elliptic integral of the second kind:
#ifdef comp_ellint_2
#undef comp_ellint_2
#endif
#define comp_ellint_2 lslboost_comp_ellint_2
#ifdef comp_ellint_2f
#undef comp_ellint_2f
#endif
#define comp_ellint_2f lslboost_comp_ellint_2f
#ifdef comp_ellint_2l
#undef comp_ellint_2l
#endif
#define comp_ellint_2l lslboost_comp_ellint_2l

// [5.2.1.6] (complete) elliptic integral of the third kind:
#ifdef comp_ellint_3
#undef comp_ellint_3
#endif
#define comp_ellint_3 lslboost_comp_ellint_3
#ifdef comp_ellint_3f
#undef comp_ellint_3f
#endif
#define comp_ellint_3f lslboost_comp_ellint_3f
#ifdef comp_ellint_3l
#undef comp_ellint_3l
#endif
#define comp_ellint_3l lslboost_comp_ellint_3l

#if 0
// [5.2.1.7] confluent hypergeometric functions:
#ifdef conf_hyper
#undef conf_hyper
#endif
#define conf_hyper lslboost_conf_hyper
#ifdef conf_hyperf
#undef conf_hyperf
#endif
#define conf_hyperf lslboost_conf_hyperf
#ifdef conf_hyperl
#undef conf_hyperl
#endif
#define conf_hyperl lslboost_conf_hyperl
#endif

// [5.2.1.8] regular modified cylindrical Bessel functions:
#ifdef cyl_bessel_i
#undef cyl_bessel_i
#endif
#define cyl_bessel_i lslboost_cyl_bessel_i
#ifdef cyl_bessel_if
#undef cyl_bessel_if
#endif
#define cyl_bessel_if lslboost_cyl_bessel_if
#ifdef cyl_bessel_il
#undef cyl_bessel_il
#endif
#define cyl_bessel_il lslboost_cyl_bessel_il

// [5.2.1.9] cylindrical Bessel functions (of the first kind):
#ifdef cyl_bessel_j
#undef cyl_bessel_j
#endif
#define cyl_bessel_j lslboost_cyl_bessel_j
#ifdef cyl_bessel_jf
#undef cyl_bessel_jf
#endif
#define cyl_bessel_jf lslboost_cyl_bessel_jf
#ifdef cyl_bessel_jl
#undef cyl_bessel_jl
#endif
#define cyl_bessel_jl lslboost_cyl_bessel_jl

// [5.2.1.10] irregular modified cylindrical Bessel functions:
#ifdef cyl_bessel_k
#undef cyl_bessel_k
#endif
#define cyl_bessel_k lslboost_cyl_bessel_k
#ifdef cyl_bessel_kf
#undef cyl_bessel_kf
#endif
#define cyl_bessel_kf lslboost_cyl_bessel_kf
#ifdef cyl_bessel_kl
#undef cyl_bessel_kl
#endif
#define cyl_bessel_kl lslboost_cyl_bessel_kl

// [5.2.1.11] cylindrical Neumann functions BOOST_MATH_C99_THROW_SPEC;
// cylindrical Bessel functions (of the second kind):
#ifdef cyl_neumann
#undef cyl_neumann
#endif
#define cyl_neumann lslboost_cyl_neumann
#ifdef cyl_neumannf
#undef cyl_neumannf
#endif
#define cyl_neumannf lslboost_cyl_neumannf
#ifdef cyl_neumannl
#undef cyl_neumannl
#endif
#define cyl_neumannl lslboost_cyl_neumannl

// [5.2.1.12] (incomplete) elliptic integral of the first kind:
#ifdef ellint_1
#undef ellint_1
#endif
#define ellint_1 lslboost_ellint_1
#ifdef ellint_1f
#undef ellint_1f
#endif
#define ellint_1f lslboost_ellint_1f
#ifdef ellint_1l
#undef ellint_1l
#endif
#define ellint_1l lslboost_ellint_1l

// [5.2.1.13] (incomplete) elliptic integral of the second kind:
#ifdef ellint_2
#undef ellint_2
#endif
#define ellint_2 lslboost_ellint_2
#ifdef ellint_2f
#undef ellint_2f
#endif
#define ellint_2f lslboost_ellint_2f
#ifdef ellint_2l
#undef ellint_2l
#endif
#define ellint_2l lslboost_ellint_2l

// [5.2.1.14] (incomplete) elliptic integral of the third kind:
#ifdef ellint_3
#undef ellint_3
#endif
#define ellint_3 lslboost_ellint_3
#ifdef ellint_3f
#undef ellint_3f
#endif
#define ellint_3f lslboost_ellint_3f
#ifdef ellint_3l
#undef ellint_3l
#endif
#define ellint_3l lslboost_ellint_3l

// [5.2.1.15] exponential integral:
#ifdef expint
#undef expint
#endif
#define expint lslboost_expint
#ifdef expintf
#undef expintf
#endif
#define expintf lslboost_expintf
#ifdef expintl
#undef expintl
#endif
#define expintl lslboost_expintl

// [5.2.1.16] Hermite polynomials:
#ifdef hermite
#undef hermite
#endif
#define hermite lslboost_hermite
#ifdef hermitef
#undef hermitef
#endif
#define hermitef lslboost_hermitef
#ifdef hermitel
#undef hermitel
#endif
#define hermitel lslboost_hermitel

#if 0
// [5.2.1.17] hypergeometric functions:
#ifdef hyperg
#undef hyperg
#endif
#define hyperg lslboost_hyperg
#ifdef hypergf
#undef hypergf
#endif
#define hypergf lslboost_hypergf
#ifdef hypergl
#undef hypergl
#endif
#define hypergl lslboost_hypergl
#endif

// [5.2.1.18] Laguerre polynomials:
#ifdef laguerre
#undef laguerre
#endif
#define laguerre lslboost_laguerre
#ifdef laguerref
#undef laguerref
#endif
#define laguerref lslboost_laguerref
#ifdef laguerrel
#undef laguerrel
#endif
#define laguerrel lslboost_laguerrel

// [5.2.1.19] Legendre polynomials:
#ifdef legendre
#undef legendre
#endif
#define legendre lslboost_legendre
#ifdef legendref
#undef legendref
#endif
#define legendref lslboost_legendref
#ifdef legendrel
#undef legendrel
#endif
#define legendrel lslboost_legendrel

// [5.2.1.20] Riemann zeta function:
#ifdef riemann_zeta
#undef riemann_zeta
#endif
#define riemann_zeta lslboost_riemann_zeta
#ifdef riemann_zetaf
#undef riemann_zetaf
#endif
#define riemann_zetaf lslboost_riemann_zetaf
#ifdef riemann_zetal
#undef riemann_zetal
#endif
#define riemann_zetal lslboost_riemann_zetal

// [5.2.1.21] spherical Bessel functions (of the first kind):
#ifdef sph_bessel
#undef sph_bessel
#endif
#define sph_bessel lslboost_sph_bessel
#ifdef sph_besself
#undef sph_besself
#endif
#define sph_besself lslboost_sph_besself
#ifdef sph_bessell
#undef sph_bessell
#endif
#define sph_bessell lslboost_sph_bessell

// [5.2.1.22] spherical associated Legendre functions:
#ifdef sph_legendre
#undef sph_legendre
#endif
#define sph_legendre lslboost_sph_legendre
#ifdef sph_legendref
#undef sph_legendref
#endif
#define sph_legendref lslboost_sph_legendref
#ifdef sph_legendrel
#undef sph_legendrel
#endif
#define sph_legendrel lslboost_sph_legendrel

// [5.2.1.23] spherical Neumann functions BOOST_MATH_C99_THROW_SPEC;
// spherical Bessel functions (of the second kind):
#ifdef sph_neumann
#undef sph_neumann
#endif
#define sph_neumann lslboost_sph_neumann
#ifdef sph_neumannf
#undef sph_neumannf
#endif
#define sph_neumannf lslboost_sph_neumannf
#ifdef sph_neumannl
#undef sph_neumannl
#endif
#define sph_neumannl lslboost_sph_neumannl

#endif // BOOST_MATH_C_MACROS_IPP
