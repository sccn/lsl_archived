/*=============================================================================
    Copyright (c) 1999-2003 Jaakko Jarvi
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_LESS_05052005_0432)
#define FUSION_LESS_05052005_0432

#include <lslboost/fusion/sequence/intrinsic/begin.hpp>
#include <lslboost/fusion/sequence/intrinsic/end.hpp>
#include <lslboost/fusion/sequence/intrinsic/size.hpp>
#include <lslboost/fusion/sequence/comparison/detail/less.hpp>
#include <lslboost/fusion/sequence/comparison/enable_comparison.hpp>

namespace lslboost { namespace fusion
{
    template <typename Seq1, typename Seq2>
    inline bool
    less(Seq1 const& a, Seq2 const& b)
    {
        return detail::sequence_less<Seq1 const, Seq2 const>::
            call(fusion::begin(a), fusion::begin(b));
    }

    namespace operators
    {
        template <typename Seq1, typename Seq2>
        inline typename
            lslboost::enable_if<
                traits::enable_comparison<Seq1, Seq2>
              , bool
            >::type
        operator<(Seq1 const& a, Seq2 const& b)
        {
            return fusion::less(a, b);
        }
    }
    using operators::operator<;
}}

#endif
