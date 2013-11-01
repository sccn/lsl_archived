//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_TR1_REGEX_HPP_INCLUDED
#  define BOOST_TR1_REGEX_HPP_INCLUDED
#  include <lslboost/tr1/detail/config.hpp>

#ifdef BOOST_HAS_TR1_REGEX

#  if defined(BOOST_HAS_INCLUDE_NEXT) && !defined(BOOST_TR1_DISABLE_INCLUDE_NEXT)
#     include_next BOOST_TR1_HEADER(regex)
#  else
#     include <lslboost/tr1/detail/config_all.hpp>
#     include BOOST_TR1_STD_HEADER(BOOST_TR1_PATH(regex))
#  endif

#else

#include <lslboost/regex.hpp>

namespace std{ namespace tr1{

// [7.5] Regex constants
namespace regex_constants {

using ::lslboost::regex_constants::syntax_option_type;
using ::lslboost::regex_constants::icase;
using ::lslboost::regex_constants::nosubs;
using ::lslboost::regex_constants::optimize;
using ::lslboost::regex_constants::collate;
using ::lslboost::regex_constants::ECMAScript;
using ::lslboost::regex_constants::basic;
using ::lslboost::regex_constants::extended;
using ::lslboost::regex_constants::awk;
using ::lslboost::regex_constants::grep;
using ::lslboost::regex_constants::egrep;

using ::lslboost::regex_constants::match_flag_type;
using ::lslboost::regex_constants::match_default;
using ::lslboost::regex_constants::match_not_bol;
using ::lslboost::regex_constants::match_not_eol;
using ::lslboost::regex_constants::match_not_bow;
using ::lslboost::regex_constants::match_not_eow;
using ::lslboost::regex_constants::match_any;
using ::lslboost::regex_constants::match_not_null;
using ::lslboost::regex_constants::match_continuous;
using ::lslboost::regex_constants::match_prev_avail;
using ::lslboost::regex_constants::format_default;
using ::lslboost::regex_constants::format_sed;
using ::lslboost::regex_constants::format_no_copy;
using ::lslboost::regex_constants::format_first_only;

using ::lslboost::regex_constants::error_type;
using ::lslboost::regex_constants::error_collate;
using ::lslboost::regex_constants::error_ctype;
using ::lslboost::regex_constants::error_escape;
using ::lslboost::regex_constants::error_backref;
using ::lslboost::regex_constants::error_brack;
using ::lslboost::regex_constants::error_paren;
using ::lslboost::regex_constants::error_brace;
using ::lslboost::regex_constants::error_badbrace;
using ::lslboost::regex_constants::error_range;
using ::lslboost::regex_constants::error_space;
using ::lslboost::regex_constants::error_badrepeat;
using ::lslboost::regex_constants::error_complexity;
using ::lslboost::regex_constants::error_stack;

} // namespace regex_constants

// [7.6] Class regex_error
using ::lslboost::regex_error;

// [7.7] Class template regex_traits
using ::lslboost::regex_traits;

// [7.8] Class template basic_regex
using ::lslboost::basic_regex;
using ::lslboost::regex;
#ifndef BOOST_NO_WREGEX
using ::lslboost::wregex;
#endif

#if !BOOST_WORKAROUND(__BORLANDC__, < 0x0582)
// [7.8.6] basic_regex swap
using ::lslboost::swap;
#endif

// [7.9] Class template sub_match
using ::lslboost::sub_match;

using ::lslboost::csub_match;
#ifndef BOOST_NO_WREGEX
using ::lslboost::wcsub_match;
#endif
using ::lslboost::ssub_match;
#ifndef BOOST_NO_WREGEX
using ::lslboost::wssub_match;
#endif

// [7.10] Class template match_results
using ::lslboost::match_results;
using ::lslboost::cmatch;
#ifndef BOOST_NO_WREGEX
using ::lslboost::wcmatch;
#endif
using ::lslboost::smatch;
#ifndef BOOST_NO_WREGEX
using ::lslboost::wsmatch;
#endif

using ::lslboost::regex_match;

// [7.11.3] Function template regex_search
using ::lslboost::regex_search;

// [7.11.4] Function template regex_replace
using ::lslboost::regex_replace;

// [7.12.1] Class template regex_iterator
using ::lslboost::regex_iterator;
using ::lslboost::cregex_iterator;
#ifndef BOOST_NO_WREGEX
using ::lslboost::wcregex_iterator;
#endif
using ::lslboost::sregex_iterator;
#ifndef BOOST_NO_WREGEX
using ::lslboost::wsregex_iterator;
#endif

// [7.12.2] Class template regex_token_iterator
using ::lslboost::regex_token_iterator;
using ::lslboost::cregex_token_iterator;
#ifndef BOOST_NO_WREGEX
using ::lslboost::wcregex_token_iterator;
#endif
using ::lslboost::sregex_token_iterator;
#ifndef BOOST_NO_WREGEX
using ::lslboost::wsregex_token_iterator;
#endif

} } // namespaces

#endif

#endif
