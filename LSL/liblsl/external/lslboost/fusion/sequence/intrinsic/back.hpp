/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_BACK_09162005_0350)
#define FUSION_BACK_09162005_0350

#include <lslboost/fusion/sequence/intrinsic_fwd.hpp>
#include <lslboost/fusion/sequence/intrinsic/end.hpp>
#include <lslboost/fusion/iterator/prior.hpp>
#include <lslboost/fusion/iterator/deref.hpp>
#include <lslboost/mpl/bool.hpp>

namespace lslboost { namespace fusion
{
    struct fusion_sequence_tag;

    namespace result_of
    {
        template <typename Sequence>
        struct back
            : result_of::deref<typename result_of::prior<typename result_of::end<Sequence>::type>::type>
        {};
    }
    
    template <typename Sequence>
    inline typename result_of::back<Sequence>::type
    back(Sequence& seq)
    {
        return *fusion::prior(fusion::end(seq));
    }

    template <typename Sequence>
    inline typename result_of::back<Sequence const>::type
    back(Sequence const& seq)
    {
        return *fusion::prior(fusion::end(seq));
    }
}}

#endif
