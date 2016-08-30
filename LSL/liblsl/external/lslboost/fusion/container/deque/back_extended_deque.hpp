/*=============================================================================
    Copyright (c) 2005-2012 Joel de Guzman
    Copyright (c) 2005-2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_BACK_EXTENDED_DEQUE_26112006_2209)
#define BOOST_FUSION_BACK_EXTENDED_DEQUE_26112006_2209

#include <lslboost/mpl/int.hpp>

#include <lslboost/fusion/sequence/intrinsic/size.hpp>
#include <lslboost/fusion/support/sequence_base.hpp>
#include <lslboost/fusion/container/deque/detail/keyed_element.hpp>

namespace lslboost { namespace fusion
{
    template <typename Deque, typename T>
    struct back_extended_deque
      : detail::keyed_element<typename Deque::next_up, T, Deque>
      , sequence_base<back_extended_deque<Deque, T> >
    {
        typedef detail::keyed_element<typename Deque::next_up, T, Deque> base;
        typedef typename Deque::next_down next_down;
        typedef mpl::int_<(Deque::next_up::value + 1)> next_up;
        typedef mpl::int_<(result_of::size<Deque>::value + 1)> size;

        template <typename Arg>
        back_extended_deque(Deque const& deque, Arg const& val)
          : base(val, deque)
        {}

#if defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
        template <typename Arg>
        back_extended_deque(Deque const& deque, Arg& val)
          : base(val, deque)
        {}
#else
        template <typename Arg>
        back_extended_deque(Deque const& deque, Arg&& val)
          : base(std::forward<Arg>(val), deque)
        {}
#endif
    };
}}

#endif
