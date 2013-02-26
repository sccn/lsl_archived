/*=============================================================================
    Copyright (c) 2005-2011 Joel de Guzman
    Copyright (c) 2005-2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_FUSION_DEQUE_26112006_1649)
#define BOOST_FUSION_DEQUE_26112006_1649

#include <lslboost/fusion/container/deque/limits.hpp>
#include <lslboost/fusion/container/deque/front_extended_deque.hpp>
#include <lslboost/fusion/container/deque/back_extended_deque.hpp>
#include <lslboost/fusion/container/deque/detail/deque_keyed_values.hpp>
#include <lslboost/fusion/container/deque/detail/deque_initial_size.hpp>
#include <lslboost/fusion/support/sequence_base.hpp>
#include <lslboost/fusion/container/deque/detail/keyed_element.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/preprocessor/repetition/enum_binary_params.hpp>
#include <lslboost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <lslboost/type_traits/add_reference.hpp>
#include <lslboost/type_traits/add_const.hpp>
#include <lslboost/type_traits/is_convertible.hpp>

///////////////////////////////////////////////////////////////////////////////
// With no decltype and variadics, we will use the C++03 version
///////////////////////////////////////////////////////////////////////////////
#if (defined(BOOST_NO_DECLTYPE)             \
  || defined(BOOST_NO_VARIADIC_TEMPLATES)   \
  || defined(BOOST_NO_RVALUE_REFERENCES))
# include <lslboost/fusion/container/deque/detail/cpp03_deque.hpp>
#else
#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 2, line: 0, output: "detail/preprocessed/deque" FUSION_MAX_DEQUE_SIZE_STR ".hpp")
#endif

///////////////////////////////////////////////////////////////////////////////
// C++11 interface
///////////////////////////////////////////////////////////////////////////////
#include <lslboost/fusion/support/sequence_base.hpp>
#include <lslboost/fusion/support/detail/access.hpp>
#include <lslboost/fusion/container/deque/detail/keyed_element.hpp>
#include <lslboost/fusion/container/deque/detail/cpp11_deque_keyed_values.hpp>
#include <lslboost/fusion/container/deque/deque_fwd.hpp>
#include <lslboost/fusion/container/deque/detail/value_at_impl.hpp>
#include <lslboost/fusion/container/deque/detail/at_impl.hpp>
#include <lslboost/fusion/container/deque/detail/begin_impl.hpp>
#include <lslboost/fusion/container/deque/detail/end_impl.hpp>
#include <lslboost/fusion/container/deque/detail/is_sequence_impl.hpp>
#include <lslboost/fusion/sequence/intrinsic/begin.hpp>

#include <lslboost/mpl/int.hpp>
#include <lslboost/utility/enable_if.hpp>
#include <lslboost/type_traits/is_convertible.hpp>

namespace lslboost { namespace fusion
{
    struct deque_tag;

    template <typename ...Elements>
    struct deque : detail::nil_keyed_element
    {
    };

    template <typename Head, typename ...Tail>
    struct deque<Head, Tail...>
      : detail::deque_keyed_values<Head, Tail...>::type
      , sequence_base<deque<Head, Tail...>>
    {
        typedef deque_tag fusion_tag;
        typedef bidirectional_traversal_tag category;
        typedef typename detail::deque_keyed_values<Head, Tail...>::type base;
        typedef mpl::int_<(sizeof ...(Tail) + 1)> size;
        typedef mpl::int_<size::value> next_up;
        typedef mpl::int_<mpl::int_<((size::value == 0) ? 0 : -1)>::type::value> next_down;
        typedef mpl::false_ is_view;

        deque()
        {}

        template <typename ...Elements>
        deque(deque<Elements...> const& seq)
          : base(seq)
        {}

        explicit deque(typename detail::call_param<Head>::type head
          , typename detail::call_param<Tail>::type... tail)
          : base(detail::deque_keyed_values<Head, Tail...>::call(head, tail...))
        {}

        template <typename Sequence>
        explicit deque(Sequence const& seq
          , typename disable_if<is_convertible<Sequence, Head> >::type* /*dummy*/ = 0)
          : base(base::from_iterator(fusion::begin(seq)))
        {}

        template <typename ...Elements>
        deque& operator=(deque<Elements...> const& rhs)
        {
            base::operator=(rhs);
            return *this;
        }

        template <typename T>
        deque& operator=(T const& rhs)
        {
            base::operator=(rhs);
            return *this;
        }
    };
}}

#endif
#endif
