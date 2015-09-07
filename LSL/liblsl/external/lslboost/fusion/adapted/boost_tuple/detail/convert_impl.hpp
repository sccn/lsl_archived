/*=============================================================================
    Copyright (c) 2012-2014 Kohei Takahashi

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_CONVERT_IMPL_10172012_0120)
#define BOOST_FUSION_CONVERT_IMPL_10172012_0120

#include <lslboost/tuple/tuple.hpp>
#include <lslboost/fusion/adapted/lslboost_tuple/detail/build_cons.hpp>
#include <lslboost/fusion/sequence/intrinsic/begin.hpp>
#include <lslboost/fusion/sequence/intrinsic/end.hpp>

namespace lslboost { namespace fusion
{
    struct lslboost_tuple_tag;

    namespace extension
    {
        template <typename T>
        struct convert_impl;

        template <>
        struct convert_impl<lslboost_tuple_tag>
        {
            template <typename Sequence>
            struct apply
            {
                typedef typename
                    detail::build_tuple_cons<
                        typename result_of::begin<Sequence>::type
                      , typename result_of::end<Sequence>::type
                    >
                build_tuple_cons;

                typedef typename build_tuple_cons::type type;

                BOOST_FUSION_GPU_ENABLED
                static type
                call(Sequence& seq)
                {
                    return build_tuple_cons::call(fusion::begin(seq), fusion::end(seq));
                }
            };
        };
    }
}}

#endif
