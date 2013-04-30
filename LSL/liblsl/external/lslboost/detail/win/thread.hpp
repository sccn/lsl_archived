//  thread.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.lslboost.org/LICENSE_1_0.txt


#ifndef BOOST_DETAIL_WIN_THREAD_HPP
#define BOOST_DETAIL_WIN_THREAD_HPP

#include <lslboost/detail/win/basic_types.hpp>
#include <lslboost/detail/win/GetCurrentThread.hpp>

namespace lslboost
{
namespace detail
{
namespace win32
{
#if defined( BOOST_USE_WINDOWS_H )
    using ::GetCurrentThreadId;
    using ::SleepEx;
    using ::Sleep;
#else
extern "C" { 
# ifndef UNDER_CE
    __declspec(dllimport) unsigned long __stdcall 
        GetCurrentThreadId(void);
    __declspec(dllimport) unsigned long __stdcall 
        SleepEx(unsigned long,int);
    __declspec(dllimport) void __stdcall 
        Sleep(unsigned long);
#else
    using ::GetCurrentThreadId;
    using ::SleepEx;
    using ::Sleep;
#endif    
}    
#endif
}
}
}

#endif // BOOST_DETAIL_WIN_THREAD_HPP
