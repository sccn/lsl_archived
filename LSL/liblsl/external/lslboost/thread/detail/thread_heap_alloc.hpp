#ifndef BOOST_THREAD_THREAD_HEAP_ALLOC_HPP
#define BOOST_THREAD_THREAD_HEAP_ALLOC_HPP

//  thread_heap_alloc.hpp
//
//  (C) Copyright 2008 Anthony Williams 
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)

#include <lslboost/thread/detail/platform.hpp>

#if defined(BOOST_THREAD_PLATFORM_WIN32)
#include <lslboost/thread/win32/thread_heap_alloc.hpp>
#elif defined(BOOST_THREAD_PLATFORM_PTHREAD)
#include <lslboost/thread/pthread/thread_heap_alloc.hpp>
#else
#error "Boost threads unavailable on this platform"
#endif


#endif
