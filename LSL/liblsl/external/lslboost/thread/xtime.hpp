// Copyright (C) 2001-2003
// William E. Kempf
// Copyright (C) 2007-8 Anthony Williams
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_XTIME_WEK070601_HPP
#define BOOST_XTIME_WEK070601_HPP

#include <lslboost/thread/detail/config.hpp>
#if defined BOOST_THREAD_USES_DATETIME

#include <lslboost/cstdint.hpp>
#include <lslboost/thread/thread_time.hpp>
#include <lslboost/date_time/posix_time/conversion.hpp>

#include <lslboost/config/abi_prefix.hpp>

namespace lslboost {

enum xtime_clock_types
{
    TIME_UTC_=1
//    TIME_TAI,
//    TIME_MONOTONIC,
//    TIME_PROCESS,
//    TIME_THREAD,
//    TIME_LOCAL,
//    TIME_SYNC,
//    TIME_RESOLUTION
};

struct xtime
{
#if defined(BOOST_NO_INT64_T)
    typedef int_fast32_t xtime_sec_t; //INT_FAST32_MIN <= sec <= INT_FAST32_MAX
#else
    typedef int_fast64_t xtime_sec_t; //INT_FAST64_MIN <= sec <= INT_FAST64_MAX
#endif

    typedef int_fast32_t xtime_nsec_t; //0 <= xtime.nsec < NANOSECONDS_PER_SECOND

    xtime_sec_t sec;
    xtime_nsec_t nsec;

    operator system_time() const
    {
        return lslboost::posix_time::from_time_t(0)+
            lslboost::posix_time::seconds(static_cast<long>(sec))+
#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS
            lslboost::posix_time::nanoseconds(nsec);
#else
        lslboost::posix_time::microseconds((nsec+500)/1000);
#endif
    }

};

inline xtime get_xtime(lslboost::system_time const& abs_time)
{
    xtime res;
    lslboost::posix_time::time_duration const time_since_epoch=abs_time-lslboost::posix_time::from_time_t(0);

    res.sec=static_cast<xtime::xtime_sec_t>(time_since_epoch.total_seconds());
    res.nsec=static_cast<xtime::xtime_nsec_t>(time_since_epoch.fractional_seconds()*(1000000000/time_since_epoch.ticks_per_second()));
    return res;
}

inline int xtime_get(struct xtime* xtp, int clock_type)
{
    if (clock_type == TIME_UTC_)
    {
        *xtp=get_xtime(get_system_time());
        return clock_type;
    }
    return 0;
}


inline int xtime_cmp(const xtime& xt1, const xtime& xt2)
{
    if (xt1.sec == xt2.sec)
        return (int)(xt1.nsec - xt2.nsec);
    else
        return (xt1.sec > xt2.sec) ? 1 : -1;
}

} // namespace lslboost

#include <lslboost/config/abi_suffix.hpp>
#endif
#endif //BOOST_XTIME_WEK070601_HPP
