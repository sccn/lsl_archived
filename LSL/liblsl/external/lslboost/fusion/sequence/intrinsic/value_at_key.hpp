/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_VALUE_AT_KEY_05052005_0229)
#define FUSION_VALUE_AT_KEY_05052005_0229

#include <lslboost/fusion/support/config.hpp>
#include <lslboost/mpl/int.hpp>
#include <lslboost/mpl/empty_base.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/mpl/or.hpp>
#include <lslboost/fusion/sequence/intrinsic_fwd.hpp>
#include <lslboost/fusion/sequence/intrinsic/has_key.hpp>
#include <lslboost/fusion/iterator/value_of_data.hpp>
#include <lslboost/fusion/algorithm/query/find.hpp>
#include <lslboost/fusion/support/tag_of.hpp>
#include <lslboost/fusion/support/category_of.hpp>

namespace lslboost { namespace fusion
{
    // Special tags:
    struct sequence_facade_tag;
    struct lslboost_array_tag; // lslboost::array tag
    struct mpl_sequence_tag; // mpl sequence tag
    struct std_pair_tag; // std::pair tag

    namespace extension
    {
        template <typename Tag>
        struct value_at_key_impl
        {
            template <typename Seq, typename Key>
            struct apply
              : result_of::value_of_data<
                    typename result_of::find<Seq, Key>::type
                >
            {};
        };

        template <>
        struct value_at_key_impl<sequence_facade_tag>
        {
            template <typename Sequence, typename Key>
            struct apply : Sequence::template value_at_key<Sequence, Key> {};
        };

        template <>
        struct value_at_key_impl<lslboost_array_tag>;

        template <>
        struct value_at_key_impl<mpl_sequence_tag>;

        template <>
        struct value_at_key_impl<std_pair_tag>;
    }

    namespace detail
    {
        template <typename Sequence, typename N, typename Tag>
        struct value_at_key_impl
            : mpl::if_<
                  mpl::or_<
                      typename extension::has_key_impl<Tag>::template apply<Sequence, N>
                    , traits::is_unbounded<Sequence>
                  >
                , typename extension::value_at_key_impl<Tag>::template apply<Sequence, N>
                , mpl::empty_base
              >::type
        {};
    }

    namespace result_of
    {
        template <typename Sequence, typename N>
        struct value_at_key
            : detail::value_at_key_impl<Sequence, N, typename detail::tag_of<Sequence>::type>
        {};
    }
}}

#endif

