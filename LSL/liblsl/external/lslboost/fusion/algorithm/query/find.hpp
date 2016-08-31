/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_FIND_05052005_1107)
#define FUSION_FIND_05052005_1107

#include <lslboost/fusion/algorithm/query/find_if_fwd.hpp>
#include <lslboost/fusion/algorithm/query/detail/find_if.hpp>
#include <lslboost/fusion/algorithm/query/detail/segmented_find.hpp>
#include <lslboost/fusion/iterator/key_of.hpp>
#include <lslboost/fusion/iterator/value_of.hpp>
#include <lslboost/fusion/support/category_of.hpp>
#include <lslboost/fusion/support/is_segmented.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/mpl/placeholders.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <lslboost/type_traits/is_const.hpp>
#include <lslboost/utility/enable_if.hpp>

namespace lslboost { namespace fusion
{
    namespace result_of
    {
        template <typename Sequence, typename T>
        struct find
          : mpl::if_<
                traits::is_segmented<Sequence>
              , detail::result_of_segmented_find<Sequence, T>
              , detail::result_of_find_if<
                    Sequence,
                    is_same<
                        typename mpl::if_<
                            traits::is_associative<Sequence>
                          , key_of<mpl::_1>
                          , value_of<mpl::_1>
                        >::type
                      , T
                    >
                >
            >::type
        {};
    }

    template <typename T, typename Sequence>
    inline typename 
        lazy_disable_if<
            is_const<Sequence>
          , result_of::find<Sequence, T>
        >::type const
    find(Sequence& seq)
    {
        typedef typename result_of::find<Sequence, T>::filter filter;
        return filter::call(seq);
    }

    template <typename T, typename Sequence>
    inline typename result_of::find<Sequence const, T>::type const
    find(Sequence const& seq)
    {
        typedef typename result_of::find<Sequence const, T>::filter filter;
        return filter::call(seq);
    }
}}

#endif
