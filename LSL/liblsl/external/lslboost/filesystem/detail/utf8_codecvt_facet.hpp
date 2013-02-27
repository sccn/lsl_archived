// Copyright (c) 2001 Ronald Garcia, Indiana University (garcia@osl.iu.edu)
// Andrew Lumsdaine, Indiana University (lums@osl.iu.edu).

// Distributed under the Boost Software License, Version 1.0.
// (See http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_FILESYSTEM_UTF8_CODECVT_FACET_HPP
#define BOOST_FILESYSTEM_UTF8_CODECVT_FACET_HPP

#include <lslboost/filesystem/config.hpp>

#define BOOST_UTF8_BEGIN_NAMESPACE \
     namespace lslboost { namespace filesystem { namespace detail {

#define BOOST_UTF8_END_NAMESPACE }}}
#define BOOST_UTF8_DECL BOOST_FILESYSTEM_DECL

#include <lslboost/detail/utf8_codecvt_facet.hpp>

#undef BOOST_UTF8_BEGIN_NAMESPACE
#undef BOOST_UTF8_END_NAMESPACE
#undef BOOST_UTF8_DECL

#endif
