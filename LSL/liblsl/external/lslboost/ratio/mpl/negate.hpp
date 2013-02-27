//  negate.hpp
//
//  (C) Copyright 2011 Vicente J. Botet Escriba
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//

#ifndef BOOST_RATIO_MPL_NEGATE_HPP
#define BOOST_RATIO_MPL_NEGATE_HPP

#include <lslboost/ratio/ratio.hpp>
#include <lslboost/ratio/mpl/numeric_cast.hpp>
#include <lslboost/mpl/negate.hpp>

namespace lslboost { 
namespace mpl {

template<>
struct negate_impl< rational_c_tag >
{
    template< typename R > struct apply
        : ratio_negate<R>
    {
    };
};    
}
}

#endif  // BOOST_RATIO_MPL_NEGATE_HPP
