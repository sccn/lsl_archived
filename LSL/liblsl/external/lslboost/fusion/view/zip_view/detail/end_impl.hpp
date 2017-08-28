/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_END_IMPL_20060123_2208)
#define FUSION_END_IMPL_20060123_2208

#include <lslboost/fusion/view/zip_view/zip_view_iterator_fwd.hpp>
#include <lslboost/fusion/sequence/intrinsic/end.hpp>
#include <lslboost/fusion/sequence/intrinsic/begin.hpp>
#include <lslboost/fusion/sequence/intrinsic/size.hpp>
#include <lslboost/fusion/sequence/intrinsic/front.hpp>
#include <lslboost/fusion/iterator/advance.hpp>
#include <lslboost/fusion/algorithm/transformation/transform.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/type_traits/is_reference.hpp>
#include <lslboost/mpl/assert.hpp>
#include <lslboost/mpl/min.hpp>

#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/type_traits/is_same.hpp>

namespace lslboost { namespace fusion {

    struct zip_view_tag;

    namespace detail
    {
        template<typename SeqRef, typename M>
        struct get_endpoint
        {
            typedef typename remove_reference<SeqRef>::type Seq;
            typedef typename result_of::begin<Seq>::type begin;
            typedef typename result_of::advance<begin, M>::type type;            
        };

        template<typename M>
        struct endpoints
        {
            template<typename T>
            struct result;

            template<typename M1, typename SeqRef>
            struct result<endpoints<M1>(SeqRef)>
                : mpl::eval_if<is_same<SeqRef, unused_type const&>,
                               mpl::identity<unused_type>,
                               get_endpoint<SeqRef, M> >
            {
                BOOST_MPL_ASSERT((is_reference<SeqRef>));
            };

            template<typename Seq>
            typename result<endpoints(Seq&)>::type
            operator()(Seq& seq) const
            {
                return fusion::advance<M>(fusion::begin(seq));
            }

            template<typename Seq>
            typename result<endpoints(Seq const&)>::type
            operator()(Seq const& seq) const
            {
                return fusion::advance<M>(fusion::begin(seq));
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
        struct end_impl;

        template<>
        struct end_impl<zip_view_tag>
        {
            template<typename Sequence>
            struct apply
            {
                typedef zip_view_iterator<
                    typename result_of::transform<typename Sequence::sequences, detail::endpoints<typename Sequence::size> >::type,
                    typename Sequence::category> type;

                static type
                call(Sequence& sequence)
                {
                    return type(
                        fusion::transform(sequence.sequences_, detail::endpoints<typename Sequence::size>()));
                }
            };
        };
    }
}}

#endif
