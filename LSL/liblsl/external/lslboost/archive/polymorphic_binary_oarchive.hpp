#ifndef BOOST_ARCHIVE_POLYMORPHIC_BINARY_OARCHIVE_HPP
#define BOOST_ARCHIVE_POLYMORPHIC_BINARY_OARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_binary_oarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org for updates, documentation, and revision history.

#include <lslboost/config.hpp>
#include <lslboost/archive/binary_oarchive.hpp>
#include <lslboost/archive/detail/polymorphic_oarchive_route.hpp>

namespace lslboost { 
namespace archive {

typedef detail::polymorphic_oarchive_route<
    binary_oarchive_impl<
        naked_binary_oarchive, 
        std::ostream::char_type, 
        std::ostream::traits_type
    >
 > polymorphic_binary_oarchive;

} // namespace archive
} // namespace lslboost

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(
    lslboost::archive::polymorphic_binary_oarchive
)

#endif // BOOST_ARCHIVE_POLYMORPHIC_BINARY_OARCHIVE_HPP

