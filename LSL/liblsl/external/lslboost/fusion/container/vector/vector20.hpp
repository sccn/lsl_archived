/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/
#if !defined(FUSION_VECTOR20_05052005_0205)
#define FUSION_VECTOR20_05052005_0205

#include <lslboost/fusion/container/vector/vector20_fwd.hpp>
#include <lslboost/fusion/support/sequence_base.hpp>
#include <lslboost/fusion/support/detail/access.hpp>
#include <lslboost/fusion/iterator/next.hpp>
#include <lslboost/fusion/iterator/deref.hpp>
#include <lslboost/fusion/sequence/intrinsic/begin.hpp>
#include <lslboost/fusion/container/vector/detail/at_impl.hpp>
#include <lslboost/fusion/container/vector/detail/value_at_impl.hpp>
#include <lslboost/fusion/container/vector/detail/begin_impl.hpp>
#include <lslboost/fusion/container/vector/detail/end_impl.hpp>

#include <lslboost/mpl/void.hpp>
#include <lslboost/mpl/int.hpp>
#include <lslboost/mpl/at.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/mpl/vector/vector20.hpp>
#include <lslboost/type_traits/is_convertible.hpp>
#include <lslboost/utility/enable_if.hpp>

#include <lslboost/preprocessor/dec.hpp>
#include <lslboost/preprocessor/iteration/iterate.hpp>
#include <lslboost/preprocessor/repetition/enum.hpp>
#include <lslboost/preprocessor/repetition/enum_shifted.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/preprocessor/repetition/enum_binary_params.hpp>
#include <lslboost/preprocessor/repetition/repeat_from_to.hpp>

#if !defined(BOOST_FUSION_DONT_USE_PREPROCESSED_FILES)
#include <lslboost/fusion/container/vector/detail/preprocessed/vector20.hpp>
#else
#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 2, line: 0, output: "detail/preprocessed/vector20.hpp")
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

namespace lslboost { namespace fusion
{
    struct vector_tag;
    struct fusion_sequence_tag;
    struct random_access_traversal_tag;

// expand vector11 to vector20
#define BOOST_PP_FILENAME_1 <lslboost/fusion/container/vector/detail/vector_n.hpp>
#define BOOST_PP_ITERATION_LIMITS (11, 20)
#include BOOST_PP_ITERATE()

}}

#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(output: null)
#endif

#endif // BOOST_FUSION_DONT_USE_PREPROCESSED_FILES

#endif

