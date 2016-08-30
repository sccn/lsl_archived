/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_HAS_KEY_09232005_1454)
#define FUSION_HAS_KEY_09232005_1454

#include <lslboost/fusion/sequence/intrinsic_fwd.hpp>
#include <lslboost/fusion/support/tag_of.hpp>
#include <lslboost/fusion/iterator/equal_to.hpp>
#include <lslboost/fusion/algorithm/query/find.hpp>
#include <lslboost/fusion/sequence/intrinsic/end.hpp>
#include <lslboost/mpl/not.hpp>

namespace lslboost { namespace fusion
{
    struct void_;

    // Special tags:
    struct sequence_facade_tag;
    struct lslboost_array_tag; // lslboost::array tag
    struct mpl_sequence_tag; // mpl sequence tag
    struct std_pair_tag; // std::pair tag

    namespace extension
    {
        template <typename Tag>
        struct has_key_impl
        {
            template <typename Seq, typename Key>
            struct apply
              : mpl::not_<
                    typename result_of::equal_to<
                        typename result_of::find<Seq, Key>::type
                      , typename result_of::end<Seq>::type
                    >::type
                >::type
            {};
        };

        template <>
        struct has_key_impl<sequence_facade_tag>
        {
            template <typename Sequence, typename Key>
            struct apply : Sequence::template has_key<Sequence, Key> {};
        };

        template <>
        struct has_key_impl<lslboost_array_tag>;

        template <>
        struct has_key_impl<mpl_sequence_tag>;

        template <>
        struct has_key_impl<std_pair_tag>;
    }
    
    namespace result_of
    {
        template <typename Sequence, typename Key>
        struct has_key 
            : extension::has_key_impl<typename detail::tag_of<Sequence>::type>::
                template apply<Sequence, Key>
        {};
    }

    template <typename Key, typename Sequence>
    inline typename result_of::has_key<Sequence, Key>::type
    has_key(Sequence const&)
    {
        typedef typename result_of::has_key<Sequence, Key>::type result;
        return result();
    }
}}

#endif

