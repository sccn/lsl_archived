// Boost uuid_serialize.hpp header file  ----------------------------------------------//

// Copyright 2007 Andy Tompkins.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

// Revision History
//  12 Nov 2007 - Initial Revision
//  25 Feb 2008 - moved to namespace lslboost::uuids::detail

#ifndef BOOST_UUID_SERIALIZE_HPP
#define BOOST_UUID_SERIALIZE_HPP

#include <lslboost/uuid/uuid.hpp>
#include <lslboost/serialization/level.hpp>

BOOST_CLASS_IMPLEMENTATION(lslboost::uuids::uuid, lslboost::serialization::primitive_type)

#endif // BOOST_UUID_SERIALIZE_HPP
