//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_TR1_UTILITY_HPP_INCLUDED
#  define BOOST_TR1_UTILITY_HPP_INCLUDED
#  include <lslboost/tr1/detail/config.hpp>

#ifdef BOOST_HAS_TR1_UTILITY

#  if defined(BOOST_HAS_INCLUDE_NEXT) && !defined(BOOST_TR1_DISABLE_INCLUDE_NEXT)
#     include_next BOOST_TR1_HEADER(utility)
#  else
#     include <lslboost/tr1/detail/config_all.hpp>
#     include BOOST_TR1_STD_HEADER(BOOST_TR1_PATH(utility))
#  endif

#else

#if defined(BOOST_TR1_USE_OLD_TUPLE)

#include <lslboost/type_traits/integral_constant.hpp>
#include <lslboost/type_traits/add_const.hpp>
#include <lslboost/type_traits/add_reference.hpp>
#include <lslboost/mpl/if.hpp>


namespace std{ namespace tr1{

template <class T> struct tuple_size; // forward declaration
template < int I, class T> struct tuple_element; // forward declaration

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
template <class T1, class T2> 
struct tuple_size< ::std::pair<T1, T2> >
   : public ::lslboost::integral_constant< ::std::size_t, 2>
{
};

template <class T1, class T2> 
struct tuple_element<0, ::std::pair<T1, T2> >
{
   typedef typename std::pair<T1, T2>::first_type type;
};

template <class T1, class T2> 
struct tuple_element<1, std::pair<T1, T2> >
{
   typedef typename std::pair<T1, T2>::second_type type;
};
#endif

namespace tuple_detail{
   template <int I, class T1, class T2>
   struct tuple_get_result
   {
      typedef typename lslboost::mpl::if_c<I==0, T1, T2>::type t1;
      typedef typename lslboost::add_reference<t1>::type type;
   };
   template <int I, class T1, class T2>
   struct const_tuple_get_result
   {
      typedef typename lslboost::mpl::if_c<I==0, T1, T2>::type t1;
# if BOOST_WORKAROUND( __BORLANDC__, BOOST_TESTED_AT( 0x582))
      // I have absolutely no idea why add_const is not working here for Borland!
      // It passes all other free-standing tests, some strange interaction going on
      typedef typename lslboost::add_reference< const t1 >::type type;
# else
      typedef typename lslboost::add_const<t1>::type t2;
      typedef typename lslboost::add_reference<t2>::type type;
# endif
   };

template<int I, class T1, class T2> 
inline typename tuple_detail::tuple_get_result<I,T1,T2>::type get(std::pair<T1, T2>& p, const ::lslboost::true_type&)
{
   return p.first;
}

template<int I, class T1, class T2> 
inline typename tuple_detail::const_tuple_get_result<I,T1,T2>::type get(const std::pair<T1, T2>& p, const ::lslboost::true_type&)
{
   return p.first;
}

template<int I, class T1, class T2> 
inline typename tuple_detail::tuple_get_result<I,T1,T2>::type get(std::pair<T1, T2>& p, const ::lslboost::false_type&)
{
   return p.second;
}

template<int I, class T1, class T2> 
inline typename tuple_detail::const_tuple_get_result<I,T1,T2>::type get(const std::pair<T1, T2>& p, const ::lslboost::false_type&)
{
   return p.second;
}

}

template<int I, class T1, class T2> 
inline typename tuple_detail::tuple_get_result<I,T1,T2>::type get(std::pair<T1, T2>& p)
{
   return tuple_detail::get<I>(p, lslboost::integral_constant<bool, I==0>());
}

template<int I, class T1, class T2> 
inline typename tuple_detail::const_tuple_get_result<I,T1,T2>::type get(const std::pair<T1, T2>& p)
{
   return tuple_detail::get<I>(p, lslboost::integral_constant<bool, I==0>());
}

} } // namespaces

#else

#include <lslboost/tr1/tuple.hpp>

#endif

#endif

#endif
