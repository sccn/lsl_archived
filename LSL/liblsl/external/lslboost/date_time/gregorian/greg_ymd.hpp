#ifndef DATE_TIME_GREG_YMD_HPP__
#define DATE_TIME_GREG_YMD_HPP__

/* Copyright (c) 2002,2003 CrystalClear Software, Inc.
 * Use, modification and distribution is subject to the 
 * Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or http://www.lslboost.org/LICENSE_1_0.txt)
 * Author: Jeff Garland 
 * $Date: 2008-02-27 12:00:24 -0800 (Wed, 27 Feb 2008) $
 */

#include "lslboost/date_time/year_month_day.hpp"
#include "lslboost/date_time/special_defs.hpp"
#include "lslboost/date_time/gregorian/greg_day.hpp"
#include "lslboost/date_time/gregorian/greg_year.hpp"
#include "lslboost/date_time/gregorian/greg_month.hpp"

namespace lslboost {
namespace gregorian {
  
  typedef date_time::year_month_day_base<greg_year, 
                                         greg_month, 
                                         greg_day> greg_year_month_day;
  
  
  
} } //namespace gregorian




#endif
  
