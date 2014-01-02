//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_TR1_TYPE_TRAITS_HPP_INCLUDED
#  define BOOST_TR1_TYPE_TRAITS_HPP_INCLUDED
#  include <lslboost/tr1/detail/config.hpp>

#ifdef BOOST_HAS_TR1_TYPE_TRAITS

#  if defined(BOOST_HAS_INCLUDE_NEXT) && !defined(BOOST_TR1_DISABLE_INCLUDE_NEXT)
#     include_next BOOST_TR1_HEADER(type_traits)
#  else
#     include <lslboost/tr1/detail/config_all.hpp>
#     include BOOST_TR1_STD_HEADER(BOOST_TR1_PATH(type_traits))
#  endif

#else
// Boost Type Traits:
#include <lslboost/type_traits.hpp>
#include <lslboost/type_traits/is_base_of_tr1.hpp>

namespace std { namespace tr1{

   using ::lslboost::integral_constant;
   using ::lslboost::true_type;
   using ::lslboost::false_type;
   using ::lslboost::is_void;
   using ::lslboost::is_integral;
   using ::lslboost::is_floating_point;
   using ::lslboost::is_array;
   using ::lslboost::is_pointer;
   using ::lslboost::is_reference;
   using ::lslboost::is_member_object_pointer;
   using ::lslboost::is_member_function_pointer;
   using ::lslboost::is_enum;
   using ::lslboost::is_union;
   using ::lslboost::is_class;
   using ::lslboost::is_function;
   using ::lslboost::is_arithmetic;
   using ::lslboost::is_fundamental;
   using ::lslboost::is_object;
   using ::lslboost::is_scalar;
   using ::lslboost::is_compound;
   using ::lslboost::is_member_pointer;
   using ::lslboost::is_const;
   using ::lslboost::is_volatile;
   using ::lslboost::is_pod;
   using ::lslboost::is_empty;
   using ::lslboost::is_polymorphic;
   using ::lslboost::is_abstract;
   using ::lslboost::has_trivial_constructor;
   using ::lslboost::has_trivial_copy;
   using ::lslboost::has_trivial_assign;
   using ::lslboost::has_trivial_destructor;
   using ::lslboost::has_nothrow_constructor;
   using ::lslboost::has_nothrow_copy;
   using ::lslboost::has_nothrow_assign;
   using ::lslboost::has_virtual_destructor;
   using ::lslboost::is_signed;
   using ::lslboost::is_unsigned;
   using ::lslboost::alignment_of;
   using ::lslboost::rank;
   using ::lslboost::extent;
   using ::lslboost::is_same;
   using ::lslboost::tr1::is_base_of;
   using ::lslboost::is_convertible;
   using ::lslboost::remove_const;
   using ::lslboost::remove_volatile;
   using ::lslboost::remove_cv;
   using ::lslboost::add_const;
   using ::lslboost::add_volatile;
   using ::lslboost::add_cv;
   using ::lslboost::remove_reference;
   using ::lslboost::add_reference;
   using ::lslboost::remove_extent;
   using ::lslboost::remove_all_extents;
   using ::lslboost::remove_pointer;
   using ::lslboost::add_pointer;
   using ::lslboost::aligned_storage;

} }

#endif

#endif
