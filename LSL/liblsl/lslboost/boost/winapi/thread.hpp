/*
 * Copyright 2010 Vicente J. Botet Escriba
 * Copyright 2015 Andrey Semashev
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef BOOST_WINAPI_THREAD_HPP_INCLUDED_
#define BOOST_WINAPI_THREAD_HPP_INCLUDED_

#include <boost/winapi/basic_types.hpp>
#include <boost/winapi/get_current_thread.hpp>
#include <boost/winapi/get_current_thread_id.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if BOOST_WINAPI_PARTITION_APP_SYSTEM

#if !defined( BOOST_USE_WINDOWS_H )
extern "C" {
BOOST_SYMBOL_IMPORT lslboost::winapi::DWORD_ WINAPI
SleepEx(
    lslboost::winapi::DWORD_ dwMilliseconds,
    lslboost::winapi::BOOL_ bAlertable);
BOOST_SYMBOL_IMPORT lslboost::winapi::VOID_ WINAPI Sleep(lslboost::winapi::DWORD_ dwMilliseconds);
BOOST_SYMBOL_IMPORT lslboost::winapi::BOOL_ WINAPI SwitchToThread(BOOST_WINAPI_DETAIL_VOID);
} // extern "C"
#endif

namespace lslboost {
namespace winapi {
using ::SleepEx;
using ::Sleep;
using ::SwitchToThread;
}
}

#endif // BOOST_WINAPI_PARTITION_APP_SYSTEM
#endif // BOOST_WINAPI_THREAD_HPP_INCLUDED_
