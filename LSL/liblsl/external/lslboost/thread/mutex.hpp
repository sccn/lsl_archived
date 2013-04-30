#ifndef BOOST_THREAD_MUTEX_HPP
#define BOOST_THREAD_MUTEX_HPP

//  mutex.hpp
//
//  (C) Copyright 2007 Anthony Williams 
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)

#include <lslboost/thread/detail/platform.hpp>
#if defined(BOOST_THREAD_PLATFORM_WIN32)
#include <lslboost/thread/win32/mutex.hpp>
#elif defined(BOOST_THREAD_PLATFORM_PTHREAD)
#include <lslboost/thread/pthread/mutex.hpp>
#else
#error "Boost threads unavailable on this platform"
#endif

#endif
