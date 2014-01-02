
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the lslboost Software License,
// Version 1.0. (See http://www.lslboost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef BOOST_FT_DETAIL_COMPONENTS_AS_MPL_SEQUENCE_HPP_INCLUDED
#define BOOST_FT_DETAIL_COMPONENTS_AS_MPL_SEQUENCE_HPP_INCLUDED

#include <lslboost/mpl/size_fwd.hpp>
#include <lslboost/mpl/empty_fwd.hpp>
#include <lslboost/mpl/front_fwd.hpp>
#include <lslboost/mpl/back_fwd.hpp>
#include <lslboost/mpl/at_fwd.hpp>
#include <lslboost/mpl/begin_end_fwd.hpp>
#include <lslboost/mpl/clear_fwd.hpp>
#include <lslboost/mpl/push_front_fwd.hpp>
#include <lslboost/mpl/pop_front_fwd.hpp>
#include <lslboost/mpl/push_back_fwd.hpp>
#include <lslboost/mpl/pop_back_fwd.hpp>

namespace lslboost { namespace mpl {

template<> struct size_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S > struct apply
    : mpl::size <typename S::types>
  { };
};
template<> struct empty_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S > struct apply
    : mpl::empty <typename S::types>
  { };
};
template<> struct front_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S > struct apply
    : mpl::front <typename S::types>
  { };
};
template<> struct back_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S > struct apply
    : mpl::back <typename S::types>
  { };
};
template<> struct at_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S, typename N > struct apply
    : mpl::at <typename S::types, N >
  { };
};
template<> struct begin_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S > struct apply
    : mpl::begin <typename S::types>
  { };
};
template<> struct end_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S > struct apply
    : mpl::end <typename S::types>
  { };
};
template<> struct clear_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S >
  struct apply
    : S
  {
    typedef apply type;
    typedef typename mpl::clear< typename S::types >::type types;
  };
};
template<>
struct push_front_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S, typename T >
  struct apply
    : S
  { 
    typedef apply type;
    typedef typename mpl::push_front< typename S::types, T >::type types;
  };
};
template<>
struct pop_front_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S >
  struct apply
    : S
  {
    typedef apply type;
    typedef typename mpl::pop_front< typename S::types >::type types; 
  };
};
template<>
struct push_back_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S, typename T >
  struct apply
    : S
  {
    typedef apply type;
    typedef typename mpl::push_back< typename S::types, T >::type types; 
  };
};
template<>
struct pop_back_impl
< function_types::detail::components_mpl_sequence_tag >
{
  template< typename S >
  struct apply
    : S
  {
    typedef apply type;
    typedef typename mpl::pop_back< typename S::types >::type types; 
  };
};

} } // namespace ::lslboost::mpl

#endif

