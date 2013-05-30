
// (C) Copyright Tobias Schwinger
//
// Use modification and distribution are subject to the lslboost Software License,
// Version 1.0. (See http://www.lslboost.org/LICENSE_1_0.txt).

//------------------------------------------------------------------------------

#ifndef BOOST_FT_DETAIL_TO_SEQUENCE_HPP_INCLUDED
#define BOOST_FT_DETAIL_TO_SEQUENCE_HPP_INCLUDED

#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/mpl/is_sequence.hpp>
#include <lslboost/mpl/placeholders.hpp>
#include <lslboost/type_traits/add_reference.hpp>

#include <lslboost/function_types/is_callable_builtin.hpp>

namespace lslboost { namespace function_types { namespace detail {

// wrap first arguments in components, if callable builtin type
template<typename T>
struct to_sequence
{
  typedef typename
   mpl::eval_if
   < is_callable_builtin<T>
   , to_sequence< components<T> >
   , mpl::identity< T >
   >::type
  type;
};

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
// reduce template instantiations, if possible
template<typename T, typename U>
struct to_sequence< components<T,U> > 
{
  typedef typename components<T,U>::types type;
};
#endif

} } } // namespace ::lslboost::function_types::detail

#endif

