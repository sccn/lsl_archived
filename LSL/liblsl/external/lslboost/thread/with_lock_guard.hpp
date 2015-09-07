// (C) Copyright 2013 Ruslan Baratov
// Copyright (C) 2014 Vicente J. Botet Escriba
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

//  See www.lslboost.org/libs/thread for documentation.

#ifndef BOOST_THREAD_WITH_LOCK_GUARD_HPP
#define BOOST_THREAD_WITH_LOCK_GUARD_HPP

#include <lslboost/thread/lock_guard.hpp>
#include <lslboost/utility/result_of.hpp>
//#include <lslboost/thread/detail/invoke.hpp>

namespace lslboost {

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) && \
    !defined(BOOST_NO_CXX11_DECLTYPE) && \
    !defined(BOOST_NO_CXX11_TRAILING_RESULT_TYPES)

/**
 * Utility to run functions in scope protected by mutex.
 *
 * Examples:
 *
 *     int func(int, int&);
 *     lslboost::mutex m;
 *     int a;
 *     int result = lslboost::with_lock_guard(m, func, 1, lslboost::ref(a));
 *
 *     // using lslboost::bind
 *     int result = lslboost::with_lock_guard(
 *         m, lslboost::bind(func, 2, lslboost::ref(a))
 *     );
 *
 *     // using lambda
 *     int a;
 *     int result = lslboost::with_lock_guard(
 *         m,
 *         [&a](int x) {
 *           a = 3;
 *           return x + 4;
 *         },
 *         5
 *     );
 */
template <class Lockable, class Function, class... Args>
typename lslboost::result_of<Function(Args...)>::type with_lock_guard(
    Lockable& m,
    BOOST_FWD_REF(Function) func,
    BOOST_FWD_REF(Args)... args
) //-> decltype(func(lslboost::forward<Args>(args)...))
{
  lslboost::lock_guard<Lockable> lock(m);
  return func(lslboost::forward<Args>(args)...);
}

#else

// Workaround versions for compilers without c++11 variadic templates support.
// (function arguments limit: 4)
// (for lambda support define BOOST_RESULT_OF_USE_DECLTYPE may be needed)

template <class Lockable, class Func>
typename lslboost::result_of<Func()>::type with_lock_guard(
    Lockable& m,
    BOOST_FWD_REF(Func) func
) {
  lslboost::lock_guard<Lockable> lock(m);
  return func();
}

template <class Lockable, class Func, class Arg>
typename lslboost::result_of<Func(Arg)>::type with_lock_guard(
    Lockable& m,
    BOOST_FWD_REF(Func) func,
    BOOST_FWD_REF(Arg) arg
) {
  lslboost::lock_guard<Lockable> lock(m);
  return func(
      lslboost::forward<Arg>(arg)
  );
}

template <class Lockable, class Func, class Arg1, class Arg2>
typename lslboost::result_of<Func(Arg1, Arg2)>::type with_lock_guard(
    Lockable& m,
    BOOST_FWD_REF(Func) func,
    BOOST_FWD_REF(Arg1) arg1,
    BOOST_FWD_REF(Arg2) arg2
) {
  lslboost::lock_guard<Lockable> lock(m);
  return func(
      lslboost::forward<Arg1>(arg1),
      lslboost::forward<Arg2>(arg2)
  );
}

template <class Lockable, class Func, class Arg1, class Arg2, class Arg3>
typename lslboost::result_of<Func(Arg1, Arg2, Arg3)>::type with_lock_guard(
    Lockable& m,
    BOOST_FWD_REF(Func) func,
    BOOST_FWD_REF(Arg1) arg1,
    BOOST_FWD_REF(Arg2) arg2,
    BOOST_FWD_REF(Arg3) arg3
) {
  lslboost::lock_guard<Lockable> lock(m);
  return func(
      lslboost::forward<Arg1>(arg1),
      lslboost::forward<Arg2>(arg2),
      lslboost::forward<Arg3>(arg3)
  );
}

template <
    class Lockable, class Func, class Arg1, class Arg2, class Arg3, class Arg4
>
typename lslboost::result_of<Func(Arg1, Arg2, Arg3, Arg4)>::type with_lock_guard(
    Lockable& m,
    BOOST_FWD_REF(Func) func,
    BOOST_FWD_REF(Arg1) arg1,
    BOOST_FWD_REF(Arg2) arg2,
    BOOST_FWD_REF(Arg3) arg3,
    BOOST_FWD_REF(Arg4) arg4
) {
  lslboost::lock_guard<Lockable> lock(m);
  return func(
      lslboost::forward<Arg1>(arg1),
      lslboost::forward<Arg2>(arg2),
      lslboost::forward<Arg3>(arg3),
      lslboost::forward<Arg4>(arg4)
  );
}

// overloads for function pointer
// (if argument is not function pointer, static assert will trigger)
template <class Lockable, class Func>
typename lslboost::result_of<
    typename lslboost::add_pointer<Func>::type()
>::type with_lock_guard(
    Lockable& m,
    Func* func
) {
  BOOST_STATIC_ASSERT(lslboost::is_function<Func>::value);

  lslboost::lock_guard<Lockable> lock(m);
  return func();
}

template <class Lockable, class Func, class Arg>
typename lslboost::result_of<
    typename lslboost::add_pointer<Func>::type(Arg)
>::type with_lock_guard(
    Lockable& m,
    Func* func,
    BOOST_FWD_REF(Arg) arg
) {
  BOOST_STATIC_ASSERT(lslboost::is_function<Func>::value);

  lslboost::lock_guard<Lockable> lock(m);
  return func(
      lslboost::forward<Arg>(arg)
  );
}

template <class Lockable, class Func, class Arg1, class Arg2>
typename lslboost::result_of<
    typename lslboost::add_pointer<Func>::type(Arg1, Arg2)
>::type with_lock_guard(
    Lockable& m,
    Func* func,
    BOOST_FWD_REF(Arg1) arg1,
    BOOST_FWD_REF(Arg2) arg2
) {
  BOOST_STATIC_ASSERT(lslboost::is_function<Func>::value);

  lslboost::lock_guard<Lockable> lock(m);
  return func(
      lslboost::forward<Arg1>(arg1),
      lslboost::forward<Arg2>(arg2)
  );
}

template <class Lockable, class Func, class Arg1, class Arg2, class Arg3>
typename lslboost::result_of<
    typename lslboost::add_pointer<Func>::type(Arg1, Arg2, Arg3)
>::type with_lock_guard(
    Lockable& m,
    Func* func,
    BOOST_FWD_REF(Arg1) arg1,
    BOOST_FWD_REF(Arg2) arg2,
    BOOST_FWD_REF(Arg3) arg3
) {
  BOOST_STATIC_ASSERT(lslboost::is_function<Func>::value);

  lslboost::lock_guard<Lockable> lock(m);
  return func(
      lslboost::forward<Arg1>(arg1),
      lslboost::forward<Arg2>(arg2),
      lslboost::forward<Arg3>(arg3)
  );
}

template <
    class Lockable, class Func, class Arg1, class Arg2, class Arg3, class Arg4
>
typename lslboost::result_of<
    typename lslboost::add_pointer<Func>::type(Arg1, Arg2, Arg3, Arg4)
>::type with_lock_guard(
    Lockable& m,
    Func* func,
    BOOST_FWD_REF(Arg1) arg1,
    BOOST_FWD_REF(Arg2) arg2,
    BOOST_FWD_REF(Arg3) arg3,
    BOOST_FWD_REF(Arg4) arg4
) {
  BOOST_STATIC_ASSERT(lslboost::is_function<Func>::value);

  lslboost::lock_guard<Lockable> lock(m);
  return func(
      lslboost::forward<Arg1>(arg1),
      lslboost::forward<Arg2>(arg2),
      lslboost::forward<Arg3>(arg3),
      lslboost::forward<Arg4>(arg4)
  );
}

#endif

} // namespace lslboost

#endif // BOOST_THREAD_WITH_LOCK_GUARD_HPP

