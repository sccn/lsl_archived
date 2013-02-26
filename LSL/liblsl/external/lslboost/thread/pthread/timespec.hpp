#ifndef BOOST_THREAD_PTHREAD_TIMESPEC_HPP
#define BOOST_THREAD_PTHREAD_TIMESPEC_HPP
//  (C) Copyright 2007-8 Anthony Williams 
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)

#include <lslboost/thread/thread_time.hpp>
#include <lslboost/date_time/posix_time/conversion.hpp>
#include <pthread.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include <lslboost/config/abi_prefix.hpp>

namespace lslboost
{
    namespace detail
    {
        inline struct timespec get_timespec(lslboost::system_time const& abs_time)
        {
            struct timespec timeout={0,0};
            lslboost::posix_time::time_duration const time_since_epoch=abs_time-lslboost::posix_time::from_time_t(0);
            
            timeout.tv_sec=time_since_epoch.total_seconds();
            timeout.tv_nsec=(long)(time_since_epoch.fractional_seconds()*(1000000000l/time_since_epoch.ticks_per_second()));
            return timeout;
        }
    }
}

#include <lslboost/config/abi_suffix.hpp>

#endif
