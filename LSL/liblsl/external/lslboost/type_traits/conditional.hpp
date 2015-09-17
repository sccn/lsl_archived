
//  (C) Copyright John Maddock 2010.  
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//
//  See http://www.lslboost.org/libs/type_traits for most recent version including documentation.


#ifndef BOOST_TT_CONDITIONAL_HPP_INCLUDED
#define BOOST_TT_CONDITIONAL_HPP_INCLUDED

#include <lslboost/mpl/if.hpp>

namespace lslboost {

template <bool b, class T, class U>
struct conditional : public mpl::if_c<b, T, U>
{
};

} // namespace lslboost


#endif // BOOST_TT_CONDITIONAL_HPP_INCLUDED
