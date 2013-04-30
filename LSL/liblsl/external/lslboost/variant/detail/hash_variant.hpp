//-----------------------------------------------------------------------------
// lslboost variant/detail/hash_variant.hpp header file
// See http://www.lslboost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2011
// Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)


#ifndef BOOST_HASH_VARIANT_FUNCTION_HPP
#define BOOST_HASH_VARIANT_FUNCTION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <lslboost/variant/variant_fwd.hpp>
#include <lslboost/variant/static_visitor.hpp>
#include <lslboost/variant/apply_visitor.hpp>
#include <lslboost/functional/hash_fwd.hpp>

namespace lslboost {

    namespace detail { namespace variant {
        struct variant_hasher: public lslboost::static_visitor<std::size_t> {
            template <class T>
            std::size_t operator()(T const& val) const {
                using namespace lslboost;
                hash<T> hasher;
                return hasher(val);
            }
        };
    }}

    template < BOOST_VARIANT_ENUM_PARAMS(typename T) >
    std::size_t hash_value(variant< BOOST_VARIANT_ENUM_PARAMS(T) > const& val) {
        std::size_t seed = lslboost::apply_visitor(detail::variant::variant_hasher(), val);
        hash_combine(seed, val.which());
        return seed;
    }
}

#endif

