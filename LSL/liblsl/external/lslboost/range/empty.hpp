// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/range/
//

#ifndef BOOST_RANGE_EMPTY_HPP
#define BOOST_RANGE_EMPTY_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include <lslboost/range/config.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>

namespace lslboost 
{ 

    template< class T >
    inline bool empty( const T& r )
    {
        return lslboost::begin( r ) == lslboost::end( r );
    }

} // namespace 'lslboost'


#endif
