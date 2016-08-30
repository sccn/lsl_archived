//  lslboost thread_clock.cpp  -----------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.lslboost.org/LICENSE_1_0.txt

//  See http://www.lslboost.org/libs/chrono for documentation.

//--------------------------------------------------------------------------------------//
#ifndef BOOST_CHRONO_DETAIL_INLINED_WIN_THREAD_CLOCK_HPP
#define BOOST_CHRONO_DETAIL_INLINED_WIN_THREAD_CLOCK_HPP

#include <lslboost/chrono/config.hpp>
#include <lslboost/chrono/thread_clock.hpp>
#include <cassert>

#include <lslboost/detail/winapi/GetLastError.hpp>
#include <lslboost/detail/winapi/GetCurrentThread.hpp>
#include <lslboost/detail/winapi/GetThreadTimes.hpp>

namespace lslboost
{
namespace chrono
{

#if !defined BOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
thread_clock::time_point thread_clock::now( system::error_code & ec )
{
    //  note that Windows uses 100 nanosecond ticks for FILETIME
    lslboost::detail::winapi::FILETIME_ creation, exit, user_time, system_time;

    if ( lslboost::detail::winapi::GetThreadTimes(
            lslboost::detail::winapi::GetCurrentThread (), &creation, &exit,
            &system_time, &user_time ) )
    {
        duration user = duration(
                ((static_cast<duration::rep>(user_time.dwHighDateTime) << 32)
                        | user_time.dwLowDateTime) * 100 );

        duration system = duration(
                ((static_cast<duration::rep>(system_time.dwHighDateTime) << 32)
                        | system_time.dwLowDateTime) * 100 );

        if (!BOOST_CHRONO_IS_THROWS(ec)) 
        {
            ec.clear();
        }
        return time_point(system+user);

    }
    else
    {
        if (BOOST_CHRONO_IS_THROWS(ec)) 
        {
            lslboost::throw_exception(
                    system::system_error( 
                            lslboost::detail::winapi::GetLastError(), 
                            BOOST_CHRONO_SYSTEM_CATEGORY, 
                            "chrono::thread_clock" ));
        } 
        else 
        {
            ec.assign( lslboost::detail::winapi::GetLastError(), BOOST_CHRONO_SYSTEM_CATEGORY );
            return thread_clock::time_point(duration(0));
        }
    }
}
#endif

thread_clock::time_point thread_clock::now() BOOST_NOEXCEPT
{

    //  note that Windows uses 100 nanosecond ticks for FILETIME
    lslboost::detail::winapi::FILETIME_ creation, exit, user_time, system_time;

    if ( lslboost::detail::winapi::GetThreadTimes( 
            lslboost::detail::winapi::GetCurrentThread (), &creation, &exit,
            &system_time, &user_time ) )
    {
        duration user   = duration(
                ((static_cast<duration::rep>(user_time.dwHighDateTime) << 32)
                        | user_time.dwLowDateTime) * 100 );

        duration system = duration(
                ((static_cast<duration::rep>(system_time.dwHighDateTime) << 32)
                        | system_time.dwLowDateTime) * 100 );

        return time_point(system+user);
    }
    else
    {
      BOOST_ASSERT(0 && "Boost::Chrono - Internal Error");
      return time_point();
    }

}

} // namespace chrono
} // namespace lslboost

#endif
