/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_FRONT_09162005_0343)
#define FUSION_FRONT_09162005_0343

#include <lslboost/fusion/sequence/intrinsic_fwd.hpp>
#include <lslboost/fusion/sequence/intrinsic/begin.hpp>
#include <lslboost/fusion/iterator/deref.hpp>
#include <lslboost/mpl/bool.hpp>

namespace lslboost { namespace fusion
{
    struct fusion_sequence_tag;

    namespace result_of
    {
        template <typename Sequence>
        struct front
            : result_of::deref<typename result_of::begin<Sequence>::type>
        {};
    }
    
    template <typename Sequence>
    inline typename result_of::front<Sequence>::type
    front(Sequence& seq)
    {
        return *fusion::begin(seq);
    }

    template <typename Sequence>
    inline typename result_of::front<Sequence const>::type
    front(Sequence const& seq)
    {
        return *fusion::begin(seq);
    }
}}

#endif
