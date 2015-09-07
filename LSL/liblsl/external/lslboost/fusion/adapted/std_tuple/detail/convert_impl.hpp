/*=============================================================================
    Copyright (c) 2012-2014 Kohei Takahashi

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_CONVERT_IMPL_10172012_0940)
#define BOOST_FUSION_CONVERT_IMPL_10172012_0940

#include <lslboost/fusion/support/config.hpp>
#include <lslboost/fusion/adapted/std_tuple/detail/build_std_tuple.hpp>
#include <lslboost/fusion/sequence/intrinsic/begin.hpp>
#include <lslboost/fusion/sequence/intrinsic/end.hpp>

namespace lslboost { namespace fusion
{
    struct std_tuple_tag;

    namespace extension
    {
        template <typename T>
        struct convert_impl;

        template <>
        struct convert_impl<std_tuple_tag>
        {
            template <typename Sequence>
            struct apply
            {
                typedef detail::build_std_tuple<
                    typename result_of::begin<Sequence>::type
                  , typename result_of::end<Sequence>::type
                > gen;

                typedef typename gen::type type;

                BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
                static type
                call(Sequence& seq)
                {
                    return gen::call(begin(seq), end(seq));
                }
            };
        };
    }
}}

#endif
