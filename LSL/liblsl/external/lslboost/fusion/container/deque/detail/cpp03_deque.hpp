/*=============================================================================
    Copyright (c) 2005-2012 Joel de Guzman
    Copyright (c) 2005-2006 Dan Marsden

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(BOOST_CPP03_FUSION_DEQUE_26112006_1649)
#define BOOST_CPP03_FUSION_DEQUE_26112006_1649

#if defined(BOOST_FUSION_HAS_CPP11_DEQUE)
#error "C++03 only! This file should not have been included"
#endif

#include <lslboost/fusion/container/deque/limits.hpp>
#include <lslboost/fusion/container/deque/front_extended_deque.hpp>
#include <lslboost/fusion/container/deque/back_extended_deque.hpp>
#include <lslboost/fusion/container/deque/detail/cpp03_deque_keyed_values.hpp>
#include <lslboost/fusion/container/deque/detail/deque_initial_size.hpp>
#include <lslboost/fusion/support/sequence_base.hpp>
#include <lslboost/fusion/container/deque/detail/keyed_element.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/preprocessor/repetition/enum_binary_params.hpp>
#include <lslboost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <lslboost/type_traits/add_reference.hpp>
#include <lslboost/type_traits/add_const.hpp>
#include <lslboost/type_traits/is_convertible.hpp>

#include <lslboost/fusion/container/deque/deque_fwd.hpp>
#include <lslboost/fusion/container/deque/detail/value_at_impl.hpp>
#include <lslboost/fusion/container/deque/detail/at_impl.hpp>
#include <lslboost/fusion/container/deque/detail/begin_impl.hpp>
#include <lslboost/fusion/container/deque/detail/end_impl.hpp>
#include <lslboost/fusion/container/deque/detail/is_sequence_impl.hpp>
#include <lslboost/fusion/sequence/intrinsic/begin.hpp>
#include <lslboost/mpl/bool.hpp>

#include <lslboost/fusion/support/sequence_base.hpp>
#include <lslboost/fusion/support/void.hpp>
#include <lslboost/utility/enable_if.hpp>

#if !defined(BOOST_FUSION_DONT_USE_PREPROCESSED_FILES)
#include <lslboost/fusion/container/deque/detail/preprocessed/deque.hpp>
#else
#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 2, line: 0, output: "detail/preprocessed/deque" FUSION_MAX_DEQUE_SIZE_STR ".hpp")
#endif

/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

    This is an auto-generated file. Do not edit!
==============================================================================*/

#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 1)
#endif

namespace lslboost { namespace fusion {

    struct deque_tag;

    template<BOOST_PP_ENUM_PARAMS(FUSION_MAX_DEQUE_SIZE, typename T)>
    struct deque
        :
        detail::deque_keyed_values<BOOST_PP_ENUM_PARAMS(FUSION_MAX_DEQUE_SIZE, T)>::type,
        sequence_base<deque<BOOST_PP_ENUM_PARAMS(FUSION_MAX_DEQUE_SIZE, T)> >
    {
        typedef deque_tag fusion_tag;
        typedef bidirectional_traversal_tag category;
        typedef typename detail::deque_keyed_values<BOOST_PP_ENUM_PARAMS(FUSION_MAX_DEQUE_SIZE, T)>::type base;
        typedef typename detail::deque_initial_size<BOOST_PP_ENUM_PARAMS(FUSION_MAX_DEQUE_SIZE, T)>::type size;
        typedef mpl::int_<size::value> next_up;
        typedef mpl::int_<
            mpl::if_<mpl::equal_to<size, mpl::int_<0> >, mpl::int_<0>, mpl::int_<-1> >::type::value> next_down;
        typedef mpl::false_ is_view;

#include <lslboost/fusion/container/deque/detail/deque_forward_ctor.hpp>

        deque()
            {}

        explicit deque(typename add_reference<typename add_const<T0>::type>::type t0)
            : base(t0, detail::nil_keyed_element())
            {}

        template<BOOST_PP_ENUM_PARAMS(FUSION_MAX_DEQUE_SIZE, typename U)>
            deque(deque<BOOST_PP_ENUM_PARAMS(FUSION_MAX_DEQUE_SIZE, U)> const& seq)
            : base(seq)
            {}

        template<typename Sequence>
            deque(Sequence const& seq, typename disable_if<is_convertible<Sequence, T0> >::type* /*dummy*/ = 0)
            : base(base::from_iterator(fusion::begin(seq)))
            {}

        template <BOOST_PP_ENUM_PARAMS(FUSION_MAX_DEQUE_SIZE, typename U)>
        deque&
        operator=(deque<BOOST_PP_ENUM_PARAMS(FUSION_MAX_DEQUE_SIZE, U)> const& rhs)
        {
            base::operator=(rhs);
            return *this;
        }

        template <typename T>
        deque&
        operator=(T const& rhs)
        {
            base::operator=(rhs);
            return *this;
        }

    };
}}

#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(output: null)
#endif

#endif // BOOST_FUSION_DONT_USE_PREPROCESSED_FILES

#endif
