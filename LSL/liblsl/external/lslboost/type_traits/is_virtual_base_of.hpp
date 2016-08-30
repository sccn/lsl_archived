//  (C) Copyright Daniel Frey and Robert Ramey 2009.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//
//  See http://www.lslboost.org/libs/type_traits for most recent version including documentation.
 
#ifndef BOOST_TT_IS_VIRTUAL_BASE_OF_HPP_INCLUDED
#define BOOST_TT_IS_VIRTUAL_BASE_OF_HPP_INCLUDED

#include <lslboost/type_traits/is_base_of.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <lslboost/mpl/and.hpp>
#include <lslboost/mpl/not.hpp>

// should be the last #include
#include <lslboost/type_traits/detail/bool_trait_def.hpp>

namespace lslboost {
namespace detail {


#ifdef BOOST_MSVC
#pragma warning( push )
#pragma warning( disable : 4584 4250)
#elif defined(__GNUC__) && (__GNUC__ >= 4)
#pragma GCC system_header
#endif

template<typename Base, typename Derived, typename tag>
struct is_virtual_base_of_impl
{
    BOOST_STATIC_CONSTANT(bool, value = false);
};

template<typename Base, typename Derived>
struct is_virtual_base_of_impl<Base, Derived, mpl::true_>
{
#ifdef __BORLANDC__
    struct lslboost_type_traits_internal_struct_X : public virtual Derived, public virtual Base 
    {
       lslboost_type_traits_internal_struct_X();
       lslboost_type_traits_internal_struct_X(const lslboost_type_traits_internal_struct_X&);
       lslboost_type_traits_internal_struct_X& operator=(const lslboost_type_traits_internal_struct_X&);
       ~lslboost_type_traits_internal_struct_X()throw();
    };
    struct lslboost_type_traits_internal_struct_Y : public virtual Derived 
    {
       lslboost_type_traits_internal_struct_Y();
       lslboost_type_traits_internal_struct_Y(const lslboost_type_traits_internal_struct_Y&);
       lslboost_type_traits_internal_struct_Y& operator=(const lslboost_type_traits_internal_struct_Y&);
       ~lslboost_type_traits_internal_struct_Y()throw();
    };
#else
    struct lslboost_type_traits_internal_struct_X : public Derived, virtual Base 
    {
       lslboost_type_traits_internal_struct_X();
       lslboost_type_traits_internal_struct_X(const lslboost_type_traits_internal_struct_X&);
       lslboost_type_traits_internal_struct_X& operator=(const lslboost_type_traits_internal_struct_X&);
       ~lslboost_type_traits_internal_struct_X()throw();
    };
    struct lslboost_type_traits_internal_struct_Y : public Derived 
    {
       lslboost_type_traits_internal_struct_Y();
       lslboost_type_traits_internal_struct_Y(const lslboost_type_traits_internal_struct_Y&);
       lslboost_type_traits_internal_struct_Y& operator=(const lslboost_type_traits_internal_struct_Y&);
       ~lslboost_type_traits_internal_struct_Y()throw();
    };
#endif
    BOOST_STATIC_CONSTANT(bool, value = (sizeof(lslboost_type_traits_internal_struct_X)==sizeof(lslboost_type_traits_internal_struct_Y)));
};

template<typename Base, typename Derived>
struct is_virtual_base_of_impl2
{
   typedef typename mpl::and_<is_base_of<Base, Derived>, mpl::not_<is_same<Base, Derived> > >::type tag_type;
   typedef is_virtual_base_of_impl<Base, Derived, tag_type> imp;
   BOOST_STATIC_CONSTANT(bool, value = imp::value);
};

#ifdef BOOST_MSVC
#pragma warning( pop )
#endif

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF2(
      is_virtual_base_of
       , Base
       , Derived
       , (::lslboost::detail::is_virtual_base_of_impl2<Base,Derived>::value) 
)

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
BOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_virtual_base_of,Base&,Derived,false)
BOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_virtual_base_of,Base,Derived&,false)
BOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_virtual_base_of,Base&,Derived&,false)
#endif

} // namespace lslboost

#include <lslboost/type_traits/detail/bool_trait_undef.hpp>

#endif
