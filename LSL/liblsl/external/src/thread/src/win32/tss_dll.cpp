// (C) Copyright Michael Glassford 2004.
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#include <lslboost/thread/detail/config.hpp>


#if defined(BOOST_HAS_WINTHREADS) && defined(BOOST_THREAD_BUILD_DLL)

    #include <lslboost/thread/detail/tss_hooks.hpp>

    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    #if defined(__BORLANDC__)
        extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE /*hInstance*/, DWORD dwReason, LPVOID /*lpReserved*/)
    #elif defined(_WIN32_WCE)
        extern "C" BOOL WINAPI DllMain(HANDLE /*hInstance*/, DWORD dwReason, LPVOID /*lpReserved*/)
    #else
        extern "C" BOOL WINAPI DllMain(HINSTANCE /*hInstance*/, DWORD dwReason, LPVOID /*lpReserved*/)
    #endif
    {
        switch(dwReason)
        {
            case DLL_PROCESS_ATTACH:
            {
                lslboost::on_process_enter();
                lslboost::on_thread_enter();
                break;
            }

            case DLL_THREAD_ATTACH:
            {
                lslboost::on_thread_enter();
                break;
            }

            case DLL_THREAD_DETACH:
            {
                lslboost::on_thread_exit();
                break;
            }

            case DLL_PROCESS_DETACH:
            {
                lslboost::on_thread_exit();
                lslboost::on_process_exit();
                break;
            }
        }

        return TRUE;
    }

namespace lslboost
{
    void tss_cleanup_implemented()
    {
        /*
        This function's sole purpose is to cause a link error in cases where
        automatic tss cleanup is not implemented by Boost.Threads as a
        reminder that user code is responsible for calling the necessary
        functions at the appropriate times (and for implementing an a
        tss_cleanup_implemented() function to eliminate the linker's
        missing symbol error).

        If Boost.Threads later implements automatic tss cleanup in cases
        where it currently doesn't (which is the plan), the duplicate
        symbol error will warn the user that their custom solution is no
        longer needed and can be removed.
        */
    }
}


#endif //defined(BOOST_HAS_WINTHREADS) && defined(BOOST_THREAD_BUILD_DLL)
