/*=============================================================================
    Copyright (c) 2010 Christopher Schmidt

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FUSION_ADAPTED_ARRAY_IS_SEQUENCE_IMPL_HPP
#define BOOST_FUSION_ADAPTED_ARRAY_IS_SEQUENCE_IMPL_HPP

#include <lslboost/mpl/bool.hpp>

namespace lslboost { namespace fusion { namespace extension
{
    template<typename>
    struct is_sequence_impl;

    template<>
    struct is_sequence_impl<po_array_tag>
    {
        template<typename Seq>
        struct apply
          : mpl::true_
        {};
    };
}}}

#endif
