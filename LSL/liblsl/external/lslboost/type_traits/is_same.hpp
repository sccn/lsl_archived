
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, 
//      Howard Hinnant and John Maddock 2000. 
//  (C) Copyright Mat Marcus, Jesse Jones and Adobe Systems Inc 2001

//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//
//  See http://www.lslboost.org/libs/type_traits for most recent version including documentation.

//    Fixed is_pointer, is_reference, is_const, is_volatile, is_same, 
//    is_member_pointer based on the Simulated Partial Specialization work 
//    of Mat Marcus and Jesse Jones. See  http://opensource.adobe.com or 
//    http://groups.yahoo.com/group/lslboost/message/5441 
//    Some workarounds in here use ideas suggested from "Generic<Programming>: 
//    Mappings between Types and Values" 
//    by Andrei Alexandrescu (see http://www.cuj.com/experts/1810/alexandr.html).


#ifndef BOOST_TT_IS_SAME_HPP_INCLUDED
#define BOOST_TT_IS_SAME_HPP_INCLUDED

#include <lslboost/type_traits/config.hpp>
#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#include <lslboost/type_traits/detail/yes_no_type.hpp>
#include <lslboost/type_traits/detail/ice_and.hpp>
#include <lslboost/type_traits/is_reference.hpp>
#endif
// should be the last #include
#include <lslboost/type_traits/detail/bool_trait_def.hpp>

namespace lslboost {

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

BOOST_TT_AUX_BOOL_TRAIT_DEF2(is_same,T,U,false)
BOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_1(typename T,is_same,T,T,true)
#if BOOST_WORKAROUND(__BORLANDC__, < 0x600)
// without this, Borland's compiler gives the wrong answer for
// references to arrays:
BOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_1(typename T,is_same,T&,T&,true)
#endif

#else // BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

namespace detail {

#ifdef BOOST_MSVC
// the following VC6 specific implementation is *NOT* legal
// C++, but has the advantage that it works for incomplete
// types.

template< typename T1 >
struct is_same_part_1
{
    template<typename T2>  struct part_2     { enum { value = false }; };
    template<>             struct part_2<T1> { enum { value = true }; };
};

template< typename T1, typename T2 >
struct is_same_impl
{
    enum { value = lslboost::detail::is_same_part_1<T1>::template part_2<T2>::value };
};

#else // generic "no-partial-specialization" version

template <typename T>
::lslboost::type_traits::yes_type
BOOST_TT_DECL is_same_tester(T*, T*);

::lslboost::type_traits::no_type
BOOST_TT_DECL is_same_tester(...);

template <typename T, typename U>
struct is_same_impl
{
   static T t;
   static U u;

   BOOST_STATIC_CONSTANT(bool, value =
      (::lslboost::type_traits::ice_and<
         (sizeof(type_traits::yes_type) == sizeof(lslboost::detail::is_same_tester(&t,&u))),
         (::lslboost::is_reference<T>::value == ::lslboost::is_reference<U>::value),
         (sizeof(T) == sizeof(U))
        >::value));
};

#endif // BOOST_MSVC

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF2(is_same,T,U,(::lslboost::detail::is_same_impl<T,U>::value))

#endif // BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

} // namespace lslboost

#include <lslboost/type_traits/detail/bool_trait_undef.hpp>

#endif  // BOOST_TT_IS_SAME_HPP_INCLUDED

