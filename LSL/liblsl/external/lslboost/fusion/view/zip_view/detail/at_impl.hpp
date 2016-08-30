/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_AT_IMPL_20060124_1933)
#define FUSION_AT_IMPL_20060124_1933

#include <lslboost/fusion/container/vector.hpp>
#include <lslboost/fusion/sequence/intrinsic/at.hpp>
#include <lslboost/fusion/container/vector/convert.hpp>
#include <lslboost/fusion/algorithm/transformation/transform.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/type_traits/is_reference.hpp>
#include <lslboost/mpl/assert.hpp>
#include <lslboost/fusion/support/unused.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/type_traits/is_same.hpp>


namespace lslboost { namespace fusion 
{
    struct zip_view_tag;

    namespace detail
    {
        template<typename N>
        struct poly_at
        {
            template<typename T>
            struct result;

            template<typename N1, typename SeqRef>
            struct result<poly_at<N1>(SeqRef)>
                : mpl::eval_if<is_same<SeqRef, unused_type const&>,
                               mpl::identity<unused_type>,
                               result_of::at<typename remove_reference<SeqRef>::type, N> >
            {
                BOOST_MPL_ASSERT((is_reference<SeqRef>));
            };

            template<typename Seq>
            typename result<poly_at(Seq&)>::type
            operator()(Seq& seq) const
            {
                return fusion::at<N>(seq);
            }

            template<typename Seq>
            typename result<poly_at(Seq const&)>::type
            operator()(Seq const& seq) const
            {
                return fusion::at<N>(seq);
            }

            unused_type operator()(unused_type const&) const
            {
                return unused_type();
            }
        };
    }

    namespace extension
    {
        template<typename Tag>
        struct at_impl;

        template<>
        struct at_impl<zip_view_tag>
        {
            template<typename Seq, typename N>
            struct apply
            {
                typedef typename result_of::as_vector<
                    typename result_of::transform<
                    typename Seq::sequences, detail::poly_at<N> >::type>::type type;

                static type
                call(Seq& seq)
                {
                    return type(
                        fusion::transform(seq.sequences_, detail::poly_at<N>()));
                }
            };
        };
    }
}}

#endif
