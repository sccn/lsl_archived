// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_DEDUCED_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DEDUCED_HPP_INCLUDED

#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/mpl/set.hpp>
#include <lslboost/mpl/empty.hpp>
#include <lslboost/type_erasure/detail/get_placeholders.hpp>
#include <lslboost/type_erasure/placeholder.hpp>

namespace lslboost {
namespace type_erasure {

/**
 * A placeholder for an associated type.  The type corresponding
 * to this placeholder is deduced by substituting placeholders
 * in the arguments of the metafunction and then evaluating it.
 * 
 * When using @ref deduced in a template context, if it is possible for
 * Metafunction to contain no placeholders at all, use the nested type,
 * to automatically evaluate it early as needed.
 */
template<class Metafunction>
struct deduced : ::lslboost::type_erasure::placeholder
{
    typedef typename ::lslboost::mpl::eval_if<
        ::lslboost::mpl::empty<
            typename ::lslboost::type_erasure::detail::get_placeholders<
                Metafunction,
                ::lslboost::mpl::set0<>
            >::type
        >,
        Metafunction,
        ::lslboost::mpl::identity<
            ::lslboost::type_erasure::deduced<Metafunction>
        >
    >::type type;
};

}
}

#endif
