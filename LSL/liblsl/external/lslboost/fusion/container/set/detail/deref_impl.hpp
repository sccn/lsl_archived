/*=============================================================================
    Copyright (c) 2009 Christopher Schmidt

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FUSION_CONTAINER_SET_DETAIL_DEREF_IMPL_HPP
#define BOOST_FUSION_CONTAINER_SET_DETAIL_DEREF_IMPL_HPP

#include <lslboost/fusion/sequence/intrinsic/at.hpp>
#include <lslboost/type_traits/is_const.hpp>

namespace lslboost { namespace fusion { namespace extension
{
    template <typename>
    struct deref_impl;

    template <>
    struct deref_impl<set_iterator_tag>
    {
        template <typename It>
        struct apply
        {
            typedef typename
                result_of::at<
                    typename mpl::if_<
                        is_const<typename It::seq_type>
                      , typename It::seq_type::storage_type const
                      , typename It::seq_type::storage_type
                    >::type
                  , typename It::index
                >::type
            type;

            static type
            call(It const& it)
            {
                return ::lslboost::fusion::at<typename It::index>(it.seq->get_data());
            }
        };
    };
}}}

#endif
