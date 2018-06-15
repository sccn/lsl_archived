// Copyright 2005 Alexander Nasonov.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef FILE_lslboost_type_traits_promote_hpp_INCLUDED
#define FILE_lslboost_type_traits_promote_hpp_INCLUDED

#include <lslboost/config.hpp>
#include <lslboost/type_traits/integral_promotion.hpp>
#include <lslboost/type_traits/floating_point_promotion.hpp>

// Should be the last #include
#include <lslboost/type_traits/detail/type_trait_def.hpp>

namespace lslboost {

namespace detail {

template<class T>
struct promote_impl
  : public integral_promotion<
        BOOST_DEDUCED_TYPENAME floating_point_promotion<T>::type
      >
{
};

}

BOOST_TT_AUX_TYPE_TRAIT_DEF1(
      promote
    , T
    , BOOST_DEDUCED_TYPENAME lslboost::detail::promote_impl<T>::type
    )
}

#include <lslboost/type_traits/detail/type_trait_undef.hpp>

#endif // #ifndef FILE_lslboost_type_traits_promote_hpp_INCLUDED

