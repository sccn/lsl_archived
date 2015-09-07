//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_PTIME_TO_TIMESPEC_HPP
#define BOOST_INTERPROCESS_DETAIL_PTIME_TO_TIMESPEC_HPP

#ifndef BOOST_CONFIG_HPP
#  include <lslboost/config.hpp>
#endif
#
#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <lslboost/interprocess/detail/posix_time_types_wrk.hpp>

namespace lslboost {

namespace interprocess {

namespace ipcdetail {

inline timespec ptime_to_timespec (const lslboost::posix_time::ptime &tm)
{
   const lslboost::posix_time::ptime epoch(lslboost::gregorian::date(1970,1,1));
   //Avoid negative absolute times
   lslboost::posix_time::time_duration duration  = (tm <= epoch) ? lslboost::posix_time::time_duration(epoch - epoch)
                                                              : lslboost::posix_time::time_duration(tm - epoch);
   timespec ts;
   ts.tv_sec  = duration.total_seconds();
   ts.tv_nsec = duration.total_nanoseconds() % 1000000000;
   return ts;
}

}  //namespace ipcdetail {

}  //namespace interprocess {

}  //namespace lslboost {

#endif   //ifndef BOOST_INTERPROCESS_DETAIL_PTIME_TO_TIMESPEC_HPP
