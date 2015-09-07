/*=============================================================================
    Copyright (c) 2014 Kohei Takahashi

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef FUSION_MAKE_LIST_10262014_0647
#define FUSION_MAKE_LIST_10262014_0647

#include <lslboost/fusion/support/config.hpp>
#include <lslboost/fusion/container/list/list.hpp>

#if !defined(BOOST_FUSION_HAS_VARIADIC_LIST)
# include <lslboost/fusion/container/generation/detail/pp_make_list.hpp>
#else

///////////////////////////////////////////////////////////////////////////////
// C++11 variadic interface
///////////////////////////////////////////////////////////////////////////////

#include <lslboost/fusion/support/detail/as_fusion_element.hpp>

namespace lslboost { namespace fusion
{
    namespace result_of
    {
        template <typename ...T>
        struct make_list
        {
            typedef list<typename detail::as_fusion_element<T>::type...> type;
        };
    }

    template <typename ...T>
    BOOST_CONSTEXPR BOOST_FUSION_GPU_ENABLED
    inline typename result_of::make_list<T...>::type
    make_list(T const&... arg)
    {
        return typename result_of::make_list<T...>::type(arg...);
    }
 }}


#endif
#endif

