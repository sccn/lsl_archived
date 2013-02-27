//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_DETAIL_PTIME_TO_TIMESPEC_HPP
#define BOOST_INTERPROCESS_DETAIL_PTIME_TO_TIMESPEC_HPP

#include <lslboost/interprocess/detail/posix_time_types_wrk.hpp>

namespace lslboost {

namespace interprocess {

namespace ipcdetail {

inline timespec ptime_to_timespec (const lslboost::posix_time::ptime &tm)
{
   const lslboost::posix_time::ptime epoch(lslboost::gregorian::date(1970,1,1));
   lslboost::posix_time::time_duration duration (tm - epoch);
   timespec ts;
   ts.tv_sec  = duration.total_seconds();
   ts.tv_nsec = duration.total_nanoseconds() % 1000000000;
   return ts;
}

}  //namespace ipcdetail {

}  //namespace interprocess {

}  //namespace lslboost {

#endif   //ifndef BOOST_INTERPROCESS_DETAIL_PTIME_TO_TIMESPEC_HPP
