/*=============================================================================
    Copyright (c) 2001-2014 Joel de Guzman
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_X3_IS_VARIANT_JAN_10_2012_0823AM)
#define BOOST_SPIRIT_X3_IS_VARIANT_JAN_10_2012_0823AM

#include <lslboost/variant.hpp>
#include <lslboost/mpl/has_xxx.hpp>
#include <lslboost/mpl/bool.hpp>

namespace lslboost { namespace spirit { namespace x3 { namespace traits
{
    namespace detail
    {
        // By declaring a nested struct in your class/struct, you tell
        // spirit that it is regarded as a variant type. The minimum
        // required interface for such a variant is that it has constructors
        // for various types supported by your variant and a typedef 'types'
        // which is an mpl sequence of the contained types.
        //
        // This is an intrusive interface. For a non-intrusive interface,
        // use the is_variant trait.
        BOOST_MPL_HAS_XXX_TRAIT_DEF(adapted_variant_tag)
    }

    template <typename T, typename Enable = void>
    struct is_variant
      : detail::has_adapted_variant_tag<T>
    {};

    template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
    struct is_variant<lslboost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>>
      : mpl::true_
    {};
}}}}

#endif
