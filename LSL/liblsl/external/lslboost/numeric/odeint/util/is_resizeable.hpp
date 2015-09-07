/*
 [auto_generated]
 lslboost/numeric/odeint/util/is_resizeable.hpp

 [begin_description]
 Metafunction to determine if a state type can resized. For usage in the steppers.
 [end_description]

 Copyright 2011-2012 Karsten Ahnert
 Copyright 2011 Mario Mulansky

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.lslboost.org/LICENSE_1_0.txt)
 */


#ifndef BOOST_NUMERIC_ODEINT_UTIL_IS_RESIZEABLE_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_UTIL_IS_RESIZEABLE_HPP_INCLUDED


#include <vector>

#include <lslboost/type_traits/integral_constant.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/fusion/include/front.hpp>
#include <lslboost/fusion/include/is_sequence.hpp>

#include <lslboost/mpl/find_if.hpp>
#include <lslboost/mpl/end.hpp>
#include <lslboost/mpl/placeholders.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/type_traits/is_same.hpp>

namespace lslboost {
namespace numeric {
namespace odeint {
   
/*
 * by default any type is not resizable
 */
template< typename Container , typename Enabler = void >
struct is_resizeable_sfinae : lslboost::false_type {};

template< typename Container >
struct is_resizeable : is_resizeable_sfinae< Container > {};



/*
 * specialization for std::vector
 */
template< class V, class A >
struct is_resizeable< std::vector< V , A  > > : lslboost::true_type {};


/*
 * sfinae specialization for fusion sequences
 */
template< typename FusionSequence >
struct is_resizeable_sfinae<
    FusionSequence ,
    typename lslboost::enable_if< typename lslboost::fusion::traits::is_sequence< FusionSequence >::type >::type >
{
    typedef typename lslboost::mpl::find_if< FusionSequence , is_resizeable< lslboost::mpl::_1 > >::type iter;
    typedef typename lslboost::mpl::end< FusionSequence >::type last;

    typedef typename lslboost::mpl::if_< lslboost::is_same< iter , last > , lslboost::false_type , lslboost::true_type >::type type;
    const static bool value = type::value;
};






} // namespace odeint
} // namespace numeric
} // namespace lslboost



#endif // BOOST_NUMERIC_ODEINT_UTIL_IS_RESIZEABLE_HPP_INCLUDED
