
//  (C) Copyright John Maddock 2007.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//
//  See http://www.lslboost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_MAKE_UNSIGNED_HPP_INCLUDED
#define BOOST_TT_MAKE_UNSIGNED_HPP_INCLUDED

#include <lslboost/mpl/if.hpp>
#include <lslboost/type_traits/is_integral.hpp>
#include <lslboost/type_traits/is_signed.hpp>
#include <lslboost/type_traits/is_unsigned.hpp>
#include <lslboost/type_traits/is_enum.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <lslboost/type_traits/remove_cv.hpp>
#include <lslboost/type_traits/is_const.hpp>
#include <lslboost/type_traits/is_volatile.hpp>
#include <lslboost/type_traits/add_const.hpp>
#include <lslboost/type_traits/add_volatile.hpp>
#include <lslboost/type_traits/detail/ice_or.hpp>
#include <lslboost/type_traits/detail/ice_and.hpp>
#include <lslboost/type_traits/detail/ice_not.hpp>
#include <lslboost/static_assert.hpp>

// should be the last #include
#include <lslboost/type_traits/detail/type_trait_def.hpp>

namespace lslboost {

namespace detail {

template <class T>
struct make_unsigned_imp
{
   BOOST_STATIC_ASSERT(
      (::lslboost::type_traits::ice_or< ::lslboost::is_integral<T>::value, ::lslboost::is_enum<T>::value>::value));
#if !BOOST_WORKAROUND(BOOST_MSVC, <=1300)
   BOOST_STATIC_ASSERT(
      (::lslboost::type_traits::ice_not< ::lslboost::is_same<
         typename remove_cv<T>::type, bool>::value>::value));
#endif

   typedef typename remove_cv<T>::type t_no_cv;
   typedef typename mpl::if_c<
      (::lslboost::type_traits::ice_and< 
         ::lslboost::is_unsigned<T>::value,
         ::lslboost::is_integral<T>::value,
         ::lslboost::type_traits::ice_not< ::lslboost::is_same<t_no_cv, char>::value>::value,
         ::lslboost::type_traits::ice_not< ::lslboost::is_same<t_no_cv, wchar_t>::value>::value,
         ::lslboost::type_traits::ice_not< ::lslboost::is_same<t_no_cv, bool>::value>::value >::value),
      T,
      typename mpl::if_c<
         (::lslboost::type_traits::ice_and< 
            ::lslboost::is_integral<T>::value,
            ::lslboost::type_traits::ice_not< ::lslboost::is_same<t_no_cv, char>::value>::value,
            ::lslboost::type_traits::ice_not< ::lslboost::is_same<t_no_cv, wchar_t>::value>::value,
            ::lslboost::type_traits::ice_not< ::lslboost::is_same<t_no_cv, bool>::value>::value>
         ::value),
         typename mpl::if_<
            is_same<t_no_cv, signed char>,
            unsigned char,
            typename mpl::if_<
               is_same<t_no_cv, short>,
               unsigned short,
               typename mpl::if_<
                  is_same<t_no_cv, int>,
                  unsigned int,
                  typename mpl::if_<
                     is_same<t_no_cv, long>,
                     unsigned long,
#if defined(BOOST_HAS_LONG_LONG)
#ifdef BOOST_HAS_INT128
                     typename mpl::if_c<
                        sizeof(t_no_cv) == sizeof(lslboost::ulong_long_type), 
                        lslboost::ulong_long_type, 
                        lslboost::uint128_type
                     >::type
#else
                     lslboost::ulong_long_type
#endif
#elif defined(BOOST_HAS_MS_INT64)
                     unsigned __int64
#else
                     unsigned long
#endif
                  >::type
               >::type
            >::type
         >::type,
         // Not a regular integer type:
         typename mpl::if_c<
            sizeof(t_no_cv) == sizeof(unsigned char),
            unsigned char,
            typename mpl::if_c<
               sizeof(t_no_cv) == sizeof(unsigned short),
               unsigned short,
               typename mpl::if_c<
                  sizeof(t_no_cv) == sizeof(unsigned int),
                  unsigned int,
                  typename mpl::if_c<
                     sizeof(t_no_cv) == sizeof(unsigned long),
                     unsigned long,
#if defined(BOOST_HAS_LONG_LONG)
#ifdef BOOST_HAS_INT128
                     typename mpl::if_c<
                        sizeof(t_no_cv) == sizeof(lslboost::ulong_long_type), 
                        lslboost::ulong_long_type, 
                        lslboost::uint128_type
                     >::type
#else
                     lslboost::ulong_long_type
#endif
#elif defined(BOOST_HAS_MS_INT64)
                     unsigned __int64
#else
                     unsigned long
#endif
                  >::type
               >::type
            >::type
         >::type
      >::type
   >::type base_integer_type;
   
   // Add back any const qualifier:
   typedef typename mpl::if_<
      is_const<T>,
      typename add_const<base_integer_type>::type,
      base_integer_type
   >::type const_base_integer_type;
   
   // Add back any volatile qualifier:
   typedef typename mpl::if_<
      is_volatile<T>,
      typename add_volatile<const_base_integer_type>::type,
      const_base_integer_type
   >::type type;
};


} // namespace detail

BOOST_TT_AUX_TYPE_TRAIT_DEF1(make_unsigned,T,typename lslboost::detail::make_unsigned_imp<T>::type)

} // namespace lslboost

#include <lslboost/type_traits/detail/type_trait_undef.hpp>

#endif // BOOST_TT_ADD_REFERENCE_HPP_INCLUDED

