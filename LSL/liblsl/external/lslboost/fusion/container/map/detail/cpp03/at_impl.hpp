/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Brandon Kohn

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_MAP_DETAIL_AT_IMPL_HPP)
#define BOOST_FUSION_MAP_DETAIL_AT_IMPL_HPP

#include <lslboost/fusion/support/detail/access.hpp>
#include <lslboost/type_traits/is_const.hpp>
#include <lslboost/mpl/at.hpp>
#include <lslboost/mpl/int.hpp>

namespace lslboost { namespace fusion
{
    struct map_tag;

    namespace extension
    {
        template <typename Tag>
        struct at_impl;

        template <>
        struct at_impl<map_tag>
        {
            template <typename Sequence, typename N>
            struct apply 
            {
                typedef typename 
                    mpl::at<typename Sequence::storage_type::types, N>::type 
                element;
                typedef typename detail::ref_result<element>::type type;
    
                static type
                call(Sequence& m)
                {
                    return m.get_data().at_impl(N());
                }
            };

            template <typename Sequence, typename N>
            struct apply<Sequence const, N>
            {
                typedef typename 
                    mpl::at<typename Sequence::storage_type::types, N>::type 
                element;
                typedef typename detail::cref_result<element>::type type;
    
                static type
                call(Sequence const& m)
                {
                    return m.get_data().at_impl(N());
                }
            };
        };
    }
}}

#endif //BOOST_FUSION_MAP_DETAIL_AT_IMPL_HPP
