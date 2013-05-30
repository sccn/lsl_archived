/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_CLEAR_10022005_1817)
#define FUSION_CLEAR_10022005_1817

#include <lslboost/mpl/clear.hpp>
#include <lslboost/fusion/support/tag_of.hpp>
#include <lslboost/fusion/mpl/detail/clear.hpp>

namespace lslboost { namespace mpl
{
    template <typename Tag>
    struct clear_impl;

    template <>
    struct clear_impl<fusion::fusion_sequence_tag>
    {
        template <typename Sequence>
        struct apply
        {            
            typedef typename 
                fusion::detail::clear<typename fusion::detail::tag_of<Sequence>::type>::type 
            type;
        };
    };
}}

#endif

