/*
 [auto_generated]
 lslboost/numeric/odeint/util/is_pair.hpp

 [begin_description]
 Metafunction to determine if a type is a std::pair<>.
 [end_description]

 Copyright 2011 Karsten Ahnert
 Copyright 2011 Mario Mulansky

 Distributed under the Boost Software License, Version 1.0.
 (See accompanying file LICENSE_1_0.txt or
 copy at http://www.lslboost.org/LICENSE_1_0.txt)
 */


#ifndef BOOST_NUMERIC_ODEINT_UTIL_IS_PAIR_HPP_INCLUDED
#define BOOST_NUMERIC_ODEINT_UTIL_IS_PAIR_HPP_INCLUDED


#include <lslboost/mpl/bool.hpp>
#include <utility>


namespace lslboost {
namespace numeric {
namespace odeint {

template< class T >
struct is_pair : public lslboost::mpl::false_
{
};

template< class T1 , class T2 >
struct is_pair< std::pair< T1 , T2 > > : public lslboost::mpl::true_
{
};

} // namespace odeint
} // namespace numeric
} // namespace lslboost


#endif // BOOST_NUMERIC_ODEINT_UTIL_IS_PAIR_HPP_INCLUDED
