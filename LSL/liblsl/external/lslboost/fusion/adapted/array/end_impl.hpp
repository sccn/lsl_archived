/*=============================================================================
    Copyright (c) 2010 Christopher Schmidt

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FUSION_ADAPTED_ARRAY_END_IMPL_HPP
#define BOOST_FUSION_ADAPTED_ARRAY_END_IMPL_HPP

#include <lslboost/fusion/iterator/basic_iterator.hpp>
#include <lslboost/type_traits/rank.hpp>
#include <lslboost/type_traits/extent.hpp>

namespace lslboost { namespace fusion { namespace extension
{
    template <typename>
    struct end_impl;

    template <>
    struct end_impl<po_array_tag>
    {
        template <typename Seq>
        struct apply
        {
            typedef
                basic_iterator<
                    po_array_iterator_tag
                  , random_access_traversal_tag
                  , Seq
                  , extent<Seq,rank<Seq>::value-1>::value
                >
            type;

            static type
            call(Seq& seq)
            {
                return type(seq,0);
            }
        };
    };
}}}

#endif
