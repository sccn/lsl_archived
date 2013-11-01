//  numeric_cast.hpp
//
//  (C) Copyright 2011 Vicente J. Botet Escriba
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//

#ifndef BOOST_RATIO_MPL_NUMERIC_CAST_HPP
#define BOOST_RATIO_MPL_NUMERIC_CAST_HPP

#include <lslboost/ratio/ratio.hpp>
#include <lslboost/mpl/integral_c.hpp>
#include <lslboost/ratio/mpl/rational_c_tag.hpp>
#include <lslboost/mpl/numeric_cast.hpp>

namespace lslboost { 
namespace mpl {

template<> struct numeric_cast< integral_c_tag,rational_c_tag >
{
    template< typename N > struct apply
        : ratio< N::value, 1 >
    {
    };
};
    
}
}

#endif  // BOOST_RATIO_MPL_NUMERIC_CAST_HPP
