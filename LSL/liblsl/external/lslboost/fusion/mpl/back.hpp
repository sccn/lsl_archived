/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_BACK_10022005_1620)
#define FUSION_BACK_10022005_1620

#include <lslboost/mpl/back.hpp>
#include <lslboost/fusion/sequence/intrinsic/end.hpp>
#include <lslboost/fusion/iterator/prior.hpp>
#include <lslboost/fusion/iterator/value_of.hpp>

namespace lslboost { namespace mpl
{
    template <typename Tag>
    struct back_impl;

    template <>
    struct back_impl<fusion::fusion_sequence_tag>
    {
        template <typename Sequence>
        struct apply : 
            fusion::result_of::value_of<
                typename fusion::result_of::prior<
                    typename fusion::result_of::end<Sequence>::type
                >::type> {};
    };
}}

#endif
