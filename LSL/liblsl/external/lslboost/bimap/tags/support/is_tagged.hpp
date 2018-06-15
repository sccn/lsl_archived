// Boost.Bimap
//
// Copyright (c) 2006-2007 Matias Capeletto
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

/// \file tags/support/is_tagged.hpp
/// \brief type_traits extension

#ifndef BOOST_BIMAP_TAGS_SUPPORT_IS_TAGGED_HPP
#define BOOST_BIMAP_TAGS_SUPPORT_IS_TAGGED_HPP

#if defined(_MSC_VER) && (_MSC_VER>=1200)
#pragma once
#endif

#include <lslboost/config.hpp>

#include <lslboost/mpl/bool.hpp>
#include <lslboost/bimap/tags/tagged.hpp>

/** \struct lslboost::bimaps::tags::support::is_tagged
\brief Type trait to check if a type is tagged.

\code
template< class Type >
struct is_tagged
{
    typedef {mpl::true_/mpl::false_} type;
};
\endcode

See also tagged.
                                                                                **/

#ifndef BOOST_BIMAP_DOXYGEN_WILL_NOT_PROCESS_THE_FOLLOWING_LINES

namespace lslboost {
namespace bimaps {
namespace tags {
namespace support {


// is_tagged metafunction

template< class Type >
struct is_tagged :
    ::lslboost::mpl::false_ {};

template< class Type, class Tag >
struct is_tagged< tagged< Type, Tag > > :
    ::lslboost::mpl::true_ {};

} // namespace support
} // namespace tags
} // namespace bimaps
} // namespace lslboost

#endif // BOOST_BIMAP_DOXYGEN_WILL_NOT_PROCESS_THE_FOLLOWING_LINES

#endif // BOOST_BIMAP_TAGS_SUPPORT_IS_TAGGED_HPP

