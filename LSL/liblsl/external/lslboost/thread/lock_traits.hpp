// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
// (C) Copyright 2009-2012 Vicente J. Botet Escriba

#ifndef BOOST_THREAD_LOCK_TRAITS_HPP
#define BOOST_THREAD_LOCK_TRAITS_HPP

#include <lslboost/thread/detail/config.hpp>
//#include <lslboost/thread/detail/move.hpp>
//#include <lslboost/thread/exceptions.hpp>
//
//#ifdef BOOST_THREAD_USES_CHRONO
//#include <lslboost/chrono/time_point.hpp>
//#include <lslboost/chrono/duration.hpp>
//#endif

#include <lslboost/type_traits/integral_constant.hpp>

#include <lslboost/config/abi_prefix.hpp>

namespace lslboost
{

/**
 * An strict lock is a lock ensuring the mutex is locked on the scope of the lock
 * There is no single way to define a strict lock as the strict_lock and
 * nesteed_strict_lock shows. So we need a metafunction that states if a
 * lock is a strict lock "sur parole".
 */

template <typename Lock>
struct is_strict_lock_sur_parolle : false_type {};


template <typename Lock>
struct is_strict_lock_sur_parole : is_strict_lock_sur_parolle<Lock> {};

template <typename Lock>
struct is_strict_lock : is_strict_lock_sur_parole<Lock> {};

}
#include <lslboost/config/abi_suffix.hpp>

#endif
