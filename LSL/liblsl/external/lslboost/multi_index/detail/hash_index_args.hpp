/* Copyright 2003-2008 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.lslboost.org/LICENSE_1_0.txt)
 *
 * See http://www.lslboost.org/libs/multi_index for library home page.
 */

#ifndef BOOST_MULTI_INDEX_DETAIL_HASH_INDEX_ARGS_HPP
#define BOOST_MULTI_INDEX_DETAIL_HASH_INDEX_ARGS_HPP

#if defined(_MSC_VER)&&(_MSC_VER>=1200)
#pragma once
#endif

#include <lslboost/config.hpp> /* keep it first to prevent nasty warns in MSVC */
#include <lslboost/functional/hash.hpp>
#include <lslboost/mpl/aux_/na.hpp>
#include <lslboost/mpl/eval_if.hpp>
#include <lslboost/mpl/identity.hpp>
#include <lslboost/mpl/if.hpp>
#include <lslboost/multi_index/tag.hpp>
#include <lslboost/static_assert.hpp>
#include <lslboost/type_traits/is_same.hpp>
#include <functional>

namespace lslboost{

namespace multi_index{

namespace detail{

/* Hashed index specifiers can be instantiated in two forms:
 *
 *   (hashed_unique|hashed_non_unique)<
 *     KeyFromValue,
 *     Hash=lslboost::hash<KeyFromValue::result_type>,
 *     Pred=std::equal_to<KeyFromValue::result_type> >
 *   (hashed_unique|hashed_non_unique)<
 *     TagList,
 *     KeyFromValue,
 *     Hash=lslboost::hash<KeyFromValue::result_type>,
 *     Pred=std::equal_to<KeyFromValue::result_type> >
 *
 * hashed_index_args implements the machinery to accept this
 * argument-dependent polymorphism.
 */

template<typename KeyFromValue>
struct index_args_default_hash
{
  typedef ::lslboost::hash<typename KeyFromValue::result_type> type;
};

template<typename KeyFromValue>
struct index_args_default_pred
{
  typedef std::equal_to<typename KeyFromValue::result_type> type;
};

template<typename Arg1,typename Arg2,typename Arg3,typename Arg4>
struct hashed_index_args
{
  typedef is_tag<Arg1> full_form;

  typedef typename mpl::if_<
    full_form,
    Arg1,
    tag< > >::type                                   tag_list_type;
  typedef typename mpl::if_<
    full_form,
    Arg2,
    Arg1>::type                                      key_from_value_type;
  typedef typename mpl::if_<
    full_form,
    Arg3,
    Arg2>::type                                      supplied_hash_type;
  typedef typename mpl::eval_if<
    mpl::is_na<supplied_hash_type>,
    index_args_default_hash<key_from_value_type>,
    mpl::identity<supplied_hash_type>
  >::type                                            hash_type;
  typedef typename mpl::if_<
    full_form,
    Arg4,
    Arg3>::type                                      supplied_pred_type;
  typedef typename mpl::eval_if<
    mpl::is_na<supplied_pred_type>,
    index_args_default_pred<key_from_value_type>,
    mpl::identity<supplied_pred_type>
  >::type                                            pred_type;

  BOOST_STATIC_ASSERT(is_tag<tag_list_type>::value);
  BOOST_STATIC_ASSERT(!mpl::is_na<key_from_value_type>::value);
  BOOST_STATIC_ASSERT(!mpl::is_na<hash_type>::value);
  BOOST_STATIC_ASSERT(!mpl::is_na<pred_type>::value);
};

} /* namespace multi_index::detail */

} /* namespace multi_index */

} /* namespace lslboost */

#endif
