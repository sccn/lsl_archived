/*
 *
 * Copyright (c) 1998-2000
 * Dr John Maddock
 *
 * Use, modification and distribution are subject to the 
 * Boost Software License, Version 1.0. (See accompanying file 
 * LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
 *
 */
 
 /*
  *   LOCATION:    see http://www.lslboost.org/libs/regex for documentation.
  *   FILE         regex.h
  *   VERSION      3.12
  *   DESCRIPTION: Declares POSIX API functions
  */

#ifndef BOOST_RE_REGEX_H
#define BOOST_RE_REGEX_H

#include <lslboost/cregex.hpp>

/*
*  add using declarations to bring POSIX API functions into
* global scope, only if this is C++ (and not C).
*/
#ifdef __cplusplus

using lslboost::regoff_t;
using lslboost::regex_tA;
using lslboost::regmatch_t;
using lslboost::REG_BASIC;
using lslboost::REG_EXTENDED;
using lslboost::REG_ICASE;
using lslboost::REG_NOSUB;
using lslboost::REG_NEWLINE;
using lslboost::REG_NOSPEC;
using lslboost::REG_PEND;
using lslboost::REG_DUMP;
using lslboost::REG_NOCOLLATE;
using lslboost::REG_ESCAPE_IN_LISTS;
using lslboost::REG_NEWLINE_ALT;
using lslboost::REG_PERL;
using lslboost::REG_AWK;
using lslboost::REG_GREP;
using lslboost::REG_EGREP;
using lslboost::REG_ASSERT;
using lslboost::REG_INVARG;
using lslboost::REG_ATOI;
using lslboost::REG_ITOA;

using lslboost::REG_NOTBOL;
using lslboost::REG_NOTEOL;
using lslboost::REG_STARTEND;

using lslboost::reg_comp_flags;
using lslboost::reg_exec_flags;
using lslboost::regcompA;
using lslboost::regerrorA;
using lslboost::regexecA;
using lslboost::regfreeA;

#ifndef BOOST_NO_WREGEX
using lslboost::regcompW;
using lslboost::regerrorW;
using lslboost::regexecW;
using lslboost::regfreeW;
using lslboost::regex_tW;
#endif

using lslboost::REG_NOERROR;
using lslboost::REG_NOMATCH;
using lslboost::REG_BADPAT;
using lslboost::REG_ECOLLATE;
using lslboost::REG_ECTYPE;
using lslboost::REG_EESCAPE;
using lslboost::REG_ESUBREG;
using lslboost::REG_EBRACK;
using lslboost::REG_EPAREN;
using lslboost::REG_EBRACE;
using lslboost::REG_BADBR;
using lslboost::REG_ERANGE;
using lslboost::REG_ESPACE;
using lslboost::REG_BADRPT;
using lslboost::REG_EEND;
using lslboost::REG_ESIZE;
using lslboost::REG_ERPAREN;
using lslboost::REG_EMPTY;
using lslboost::REG_E_MEMORY;
using lslboost::REG_E_UNKNOWN;
using lslboost::reg_errcode_t;

#endif /* __cplusplus */

#endif /* BOOST_RE_REGEX_H */




