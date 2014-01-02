/*=============================================================================
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_SEGMENTED_SIZE_08112006_1141)
#define BOOST_FUSION_SEGMENTED_SIZE_08112006_1141

#include <lslboost/type_traits/add_const.hpp>
#include <lslboost/type_traits/remove_reference.hpp>
#include <lslboost/mpl/fold.hpp>
#include <lslboost/mpl/plus.hpp>
#include <lslboost/mpl/size_t.hpp>
#include <lslboost/mpl/placeholders.hpp>
#include <lslboost/fusion/sequence/intrinsic_fwd.hpp>
#include <lslboost/fusion/mpl/begin.hpp>
#include <lslboost/fusion/mpl/end.hpp>
#include <lslboost/fusion/support/is_segmented.hpp>

namespace lslboost { namespace fusion { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    // calculates the size of any segmented data structure.
    template<typename Sequence>
    struct segmented_size;

    ///////////////////////////////////////////////////////////////////////////
    template<typename Sequence, bool IsSegmented = traits::is_segmented<Sequence>::value>
    struct segmented_size_impl
      : mpl::fold<
            typename remove_reference<
                typename add_const<
                    typename result_of::segments<Sequence>::type
                >::type
            >::type
          , mpl::size_t<0>
          , mpl::plus<mpl::_1, segmented_size<remove_reference<mpl::_2> > >
        >::type
    {};

    template<typename Sequence>
    struct segmented_size_impl<Sequence, false>
      : result_of::size<Sequence>::type
    {};

    template<typename Sequence>
    struct segmented_size
      : segmented_size_impl<Sequence>
    {};

}}}

#endif
