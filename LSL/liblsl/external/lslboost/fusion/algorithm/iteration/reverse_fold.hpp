/*=============================================================================
    Copyright (c) 2010 Christopher Schmidt
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_FUSION_ALGORITHM_ITERATION_REVERSE_FOLD_HPP
#define BOOST_FUSION_ALGORITHM_ITERATION_REVERSE_FOLD_HPP

#include <lslboost/fusion/algorithm/iteration/reverse_fold_fwd.hpp>
#include <lslboost/config.hpp>
#include <lslboost/fusion/sequence/intrinsic/begin.hpp>
#include <lslboost/fusion/sequence/intrinsic/end.hpp>
#include <lslboost/fusion/sequence/intrinsic/empty.hpp>
#include <lslboost/fusion/sequence/intrinsic/size.hpp>
#include <lslboost/fusion/support/is_segmented.hpp>
#include <lslboost/fusion/iterator/equal_to.hpp>
#include <lslboost/fusion/iterator/deref.hpp>
#include <lslboost/fusion/iterator/value_of.hpp>
#include <lslboost/fusion/iterator/prior.hpp>
#include <lslboost/fusion/iterator/next.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/mpl/bool.hpp>
#include <lslboost/utility/result_of.hpp>
#include <lslboost/type_traits/add_const.hpp>
#include <lslboost/type_traits/add_reference.hpp>

#define BOOST_FUSION_REVERSE_FOLD

#if !defined(BOOST_FUSION_DONT_USE_PREPROCESSED_FILES)
#include <lslboost/fusion/algorithm/iteration/detail/preprocessed/reverse_fold.hpp>
#else
#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 2, line: 0, output: "detail/preprocessed/reverse_fold.hpp")
#endif

/*=============================================================================
    Copyright (c) 2009-2010 Christopher Schmidt
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

    This is an auto-generated file. Do not edit!
==============================================================================*/

#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(preserve: 1)
#endif

#include <lslboost/fusion/algorithm/iteration/detail/fold.hpp>

#if defined(__WAVE__) && defined(BOOST_FUSION_CREATE_PREPROCESSED_FILES)
#pragma wave option(output: null)
#endif

#endif // BOOST_FUSION_DONT_USE_PREPROCESSED_FILES

#undef BOOST_FUSION_REVERSE_FOLD

#endif
