/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2005-2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_DETAIL_IS_MPL_SEQUENCE_29122006_1105)
#define FUSION_DETAIL_IS_MPL_SEQUENCE_29122006_1105

#include <lslboost/fusion/support/sequence_base.hpp>
#include <lslboost/mpl/is_sequence.hpp>
#include <lslboost/mpl/and.hpp>
#include <lslboost/mpl/not.hpp>
#include <lslboost/type_traits/is_convertible.hpp>

namespace lslboost { namespace fusion { namespace detail
{
    template <typename T>
    struct is_mpl_sequence
        : mpl::and_<
            mpl::not_<is_convertible<T, from_sequence_convertible_type> >
          , mpl::is_sequence<T> >
    {};
}}}

#endif
