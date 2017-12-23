/*
 * Copyright 2010 Vicente J. Botet Escriba
 * Copyright 2015 Andrey Semashev
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef BOOST_WINAPI_HANDLES_HPP_INCLUDED_
#define BOOST_WINAPI_HANDLES_HPP_INCLUDED_

#include <boost/winapi/basic_types.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined( BOOST_USE_WINDOWS_H )
extern "C" {
BOOST_SYMBOL_IMPORT lslboost::winapi::BOOL_ WINAPI
CloseHandle(lslboost::winapi::HANDLE_ handle);

BOOST_SYMBOL_IMPORT lslboost::winapi::BOOL_ WINAPI
DuplicateHandle(
    lslboost::winapi::HANDLE_ hSourceProcessHandle,
    lslboost::winapi::HANDLE_ hSourceHandle,
    lslboost::winapi::HANDLE_ hTargetProcessHandle,
    lslboost::winapi::HANDLE_* lpTargetHandle,
    lslboost::winapi::DWORD_ dwDesiredAccess,
    lslboost::winapi::BOOL_ bInheritHandle,
    lslboost::winapi::DWORD_ dwOptions);

#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN10
BOOST_SYMBOL_IMPORT lslboost::winapi::BOOL_ WINAPI
CompareObjectHandles(
    lslboost::winapi::HANDLE_ hFirstObjectHandle,
    lslboost::winapi::HANDLE_ hSecondObjectHandle);
#endif
} // extern "C"
#endif

namespace lslboost {
namespace winapi {

using ::CloseHandle;
using ::DuplicateHandle;

#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN10
using ::CompareObjectHandles;
#endif

#if defined( BOOST_USE_WINDOWS_H )
const DWORD_ DUPLICATE_CLOSE_SOURCE_ = DUPLICATE_CLOSE_SOURCE;
const DWORD_ DUPLICATE_SAME_ACCESS_ = DUPLICATE_SAME_ACCESS;
const HANDLE_ INVALID_HANDLE_VALUE_ = INVALID_HANDLE_VALUE;
#else
const DWORD_ DUPLICATE_CLOSE_SOURCE_ = 1;
const DWORD_ DUPLICATE_SAME_ACCESS_ = 2;
const HANDLE_ INVALID_HANDLE_VALUE_ = (HANDLE_)(-1);
#endif

const DWORD_ duplicate_close_source = DUPLICATE_CLOSE_SOURCE_;
const DWORD_ duplicate_same_access = DUPLICATE_SAME_ACCESS_;
const HANDLE_ invalid_handle_value = INVALID_HANDLE_VALUE_;

}
}

#endif // BOOST_WINAPI_HANDLES_HPP_INCLUDED_
