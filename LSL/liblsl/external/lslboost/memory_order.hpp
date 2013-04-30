#ifndef BOOST_MEMORY_ORDER_HPP_INCLUDED
#define BOOST_MEMORY_ORDER_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  lslboost/memory_order.hpp
//
//  Defines enum lslboost::memory_order per the C++0x working draft
//
//  Copyright (c) 2008, 2009 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)


namespace lslboost
{

//
// Enum values are chosen so that code that needs to insert
// a trailing fence for acquire semantics can use a single
// test such as:
//
// if( mo & memory_order_acquire ) { ...fence... }
//
// For leading fences one can use:
//
// if( mo & memory_order_release ) { ...fence... }
//
// Architectures such as Alpha that need a fence on consume
// can use:
//
// if( mo & ( memory_order_acquire | memory_order_consume ) ) { ...fence... }
//

enum memory_order
{
    memory_order_relaxed = 0,
    memory_order_acquire = 1,
    memory_order_release = 2,
    memory_order_acq_rel = 3, // acquire | release
    memory_order_seq_cst = 7, // acq_rel | 4
    memory_order_consume = 8
};

} // namespace lslboost

#endif // #ifndef BOOST_MEMORY_ORDER_HPP_INCLUDED
