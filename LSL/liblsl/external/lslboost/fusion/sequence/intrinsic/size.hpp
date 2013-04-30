/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_SIZE_05052005_0214)
#define FUSION_SIZE_05052005_0214

#include <lslboost/utility/enable_if.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/mpl/int.hpp>
#include <lslboost/fusion/sequence/intrinsic_fwd.hpp>
#include <lslboost/fusion/support/tag_of.hpp>
#include <lslboost/fusion/support/is_segmented.hpp>
#include <lslboost/fusion/sequence/intrinsic/detail/segmented_size.hpp>

namespace lslboost { namespace fusion
{
    // Special tags:
    struct sequence_facade_tag;
    struct lslboost_tuple_tag; // lslboost::tuples::tuple tag
    struct lslboost_array_tag; // lslboost::array tag
    struct mpl_sequence_tag; // mpl sequence tag
    struct std_pair_tag; // std::pair tag

    namespace extension
    {
        template <typename Tag>
        struct size_impl
        {
            template<typename Sequence>
            struct unsegmented_size : Sequence::size {};

            template <typename Sequence>
            struct apply
              : mpl::if_<
                    traits::is_segmented<Sequence>
                  , detail::segmented_size<Sequence>
                  , unsegmented_size<Sequence>
                >::type
            {};
        };

        template <>
        struct size_impl<sequence_facade_tag>
        {
            template <typename Sequence>
            struct apply : Sequence::template size<Sequence> {};
        };
 
        template <>
        struct size_impl<lslboost_tuple_tag>;
 
        template <>
        struct size_impl<lslboost_array_tag>;

        template <>
        struct size_impl<mpl_sequence_tag>;

        template <>
        struct size_impl<std_pair_tag>;
    }

    namespace result_of
    {
        template <typename Sequence>
        struct size
            : extension::size_impl<typename detail::tag_of<Sequence>::type>::
        template apply<Sequence>

        {
            typedef typename extension::size_impl<typename detail::tag_of<Sequence>::type>::
            template apply<Sequence>::type size_application;
            BOOST_STATIC_CONSTANT(int, value = size_application::value);
        };
    }

    template <typename Sequence>
    inline typename result_of::size<Sequence>::type
    size(Sequence const&)
    {
        typedef typename result_of::size<Sequence>::type result;
        return result();
    }
}}

#endif
