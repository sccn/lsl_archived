//  times.hpp
//
//  (C) Copyright 2011 Vicente J. Botet Escriba
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//

#ifndef BOOST_RATIO_MPL_TIMES_HPP
#define BOOST_RATIO_MPL_TIMES_HPP

#include <lslboost/ratio/ratio.hpp>
#include <lslboost/ratio/mpl/numeric_cast.hpp>
#include <lslboost/mpl/times.hpp>

namespace lslboost { 
namespace mpl {

template<>
struct times_impl< rational_c_tag,rational_c_tag >
{
    template< typename R1, typename R2 > struct apply
        : ratio_multiply<R1, R2>
    {
    };
};    
}
}

#endif  // BOOST_RATIO_MPL_TIMES_HPP
