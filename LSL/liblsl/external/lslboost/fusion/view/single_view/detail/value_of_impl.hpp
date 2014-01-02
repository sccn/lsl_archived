/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_SINGLE_VIEW_VALUE_OF_IMPL_05052005_0324)
#define BOOST_FUSION_SINGLE_VIEW_VALUE_OF_IMPL_05052005_0324

#include <lslboost/mpl/int.hpp>
#include <lslboost/mpl/assert.hpp>
#include <lslboost/mpl/equal_to.hpp>

namespace lslboost { namespace fusion
{
    struct single_view_iterator_tag;

    namespace extension
    {
        template <typename Tag>
        struct value_of_impl;

        template <>
        struct value_of_impl<single_view_iterator_tag>
        {
            template <typename Iterator>
            struct apply
            {
                BOOST_MPL_ASSERT((mpl::equal_to<typename Iterator::position, mpl::int_<0> >));
                typedef typename Iterator::value_type type;
            };
        };
    }
}}

#endif


