/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_END_IMPL_09272006_0721)
#define BOOST_FUSION_END_IMPL_09272006_0721

#include <lslboost/fusion/adapted/lslboost_tuple/lslboost_tuple_iterator.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/type_traits/is_const.hpp>

namespace lslboost { namespace tuples
{
    struct null_type;
}}
    
namespace lslboost { namespace fusion
{
    struct lslboost_tuple_tag;

    namespace extension
    {
        template <typename Tag>
        struct end_impl;

        template <>
        struct end_impl<lslboost_tuple_tag>
        {
            template <typename Sequence>
            struct apply 
            {
                typedef 
                    lslboost_tuple_iterator<
                        typename mpl::if_<
                            is_const<Sequence>
                          , tuples::null_type const
                          , tuples::null_type
                        >::type
                    > 
                type;

                static type
                call(Sequence& seq)
                {
                    return type(seq);
                }
            };
        };
    }
}}

#endif
