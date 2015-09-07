// Boost.TypeErasure library
//
// Copyright 2012 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_DETAIL_GET_SIGNATURE_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_GET_SIGNATURE_HPP_INCLUDED

#include <lslboost/typeof/typeof.hpp>
#include <lslboost/type_traits/remove_pointer.hpp>

namespace lslboost {
namespace type_erasure {
namespace detail {

template<class Concept>
struct get_signature {
    BOOST_TYPEOF_NESTED_TYPEDEF_TPL(nested, &Concept::apply)
    typedef typename lslboost::remove_pointer<
        typename nested::type
    >::type type;
};

}
}
}

#endif
