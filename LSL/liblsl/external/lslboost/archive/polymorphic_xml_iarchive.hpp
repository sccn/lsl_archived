#ifndef BOOST_ARCHIVE_POLYMORPHIC_XML_IARCHIVE_HPP
#define BOOST_ARCHIVE_POLYMORPHIC_XML_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// polymorphic_xml_iarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org for updates, documentation, and revision history.

#include <lslboost/config.hpp>
#include <lslboost/archive/xml_iarchive.hpp>
#include <lslboost/archive/detail/polymorphic_iarchive_route.hpp>

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace lslboost { 
namespace archive {

class polymorphic_xml_iarchive : 
    public detail::polymorphic_iarchive_route<naked_xml_iarchive>
{
public:
    polymorphic_xml_iarchive(std::istream & is, unsigned int flags = 0) :
        detail::polymorphic_iarchive_route<naked_xml_iarchive>(is, flags)
    {}
    ~polymorphic_xml_iarchive(){}
};

} // namespace archive
} // namespace lslboost

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(
    lslboost::archive::polymorphic_xml_iarchive
)

#endif // BOOST_ARCHIVE_POLYMORPHIC_XML_IARCHIVE_HPP

