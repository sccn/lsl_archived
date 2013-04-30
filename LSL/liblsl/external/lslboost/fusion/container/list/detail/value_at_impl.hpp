/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_VALUE_AT_IMPL_07172005_0952)
#define FUSION_VALUE_AT_IMPL_07172005_0952

#include <lslboost/fusion/support/detail/access.hpp>
#include <lslboost/type_traits/is_const.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/bool.hpp>

namespace lslboost { namespace fusion
{
    struct cons_tag;

    namespace extension
    {
        template <typename Tag>
        struct value_at_impl;

        template <>
        struct value_at_impl<cons_tag>
        {
            template <typename Sequence, typename N>
            struct apply 
            {
                typedef typename 
                    mpl::eval_if<
                        mpl::bool_<N::value == 0>
                      , mpl::identity<typename Sequence::car_type>
                      , apply<typename Sequence::cdr_type, mpl::int_<N::value-1> >
                    >::type
                type;
            };
        };
    }
}}

#endif
