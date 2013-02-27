/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_POP_FRONT_09172005_1115)
#define FUSION_POP_FRONT_09172005_1115

#include <lslboost/fusion/view/iterator_range/iterator_range.hpp>
#include <lslboost/fusion/sequence/intrinsic/begin.hpp>
#include <lslboost/fusion/sequence/intrinsic/end.hpp>
#include <lslboost/fusion/iterator/next.hpp>

namespace lslboost { namespace fusion
{
    namespace result_of
    {
        template <typename Sequence>
        struct pop_front
        {
            typedef 
                iterator_range<
                    typename next<
                        typename begin<Sequence>::type
                    >::type
                  , typename end<Sequence>::type
                > 
            type;
        };
    }

    template <typename Sequence>
    inline typename result_of::pop_front<Sequence const>::type
    pop_front(Sequence const& seq)
    {
        typedef typename result_of::pop_front<Sequence const>::type result;
        return result(fusion::next(fusion::begin(seq)), fusion::end(seq));
    }
}}

#endif

