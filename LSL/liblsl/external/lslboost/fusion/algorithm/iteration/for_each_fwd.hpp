/*=============================================================================
    Copyright (c) 2011 Eric Niebler

    Distributed under the Boost Software License, Version 1.0. (See accompanying 
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_FOR_EACH_FWD_HPP_INCLUDED)
#define BOOST_FUSION_FOR_EACH_FWD_HPP_INCLUDED

#include <lslboost/fusion/support/is_sequence.hpp>
#include <lslboost/utility/enable_if.hpp>

namespace lslboost { namespace fusion
{
    namespace result_of
    {
        template <typename Sequence, typename F>
        struct for_each;
    }

    template <typename Sequence, typename F>
    inline
    typename
        enable_if<
            traits::is_sequence<Sequence>
          , void
        >::type
    for_each(Sequence& seq, F const& f);

    template <typename Sequence, typename F>
    inline
    typename
        enable_if<
            traits::is_sequence<Sequence>
          , void
        >::type
    for_each(Sequence const& seq, F const& f);
}}

#endif
