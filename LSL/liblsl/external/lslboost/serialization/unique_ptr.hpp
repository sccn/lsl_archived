#ifndef  BOOST_SERIALIZATION_UNIQUE_PTR_HPP
#define BOOST_SERIALIZATION_UNIQUE_PTR_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// unique_ptr.hpp:

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org for updates, documentation, and revision history.
#include <memory>
#include <lslboost/serialization/split_free.hpp>
#include <lslboost/serialization/nvp.hpp>

namespace lslboost {
namespace serialization {

/////////////////////////////////////////////////////////////
// implement serialization for unique_ptr< T >
// note: this must be added to the lslboost namespace in order to
// be called by the library
template<class Archive, class T>
inline void save(
    Archive & ar,
    const std::unique_ptr< T > &t,
    const unsigned int file_version
){
    // only the raw pointer has to be saved
    // the ref count is rebuilt automatically on load
    const T * const tx = t.get();
    ar << BOOST_SERIALIZATION_NVP(tx);
}

template<class Archive, class T>
inline void load(
    Archive & ar,
    std::unique_ptr< T > &t,
    const unsigned int file_version
){
    T *tx;
    ar >> BOOST_SERIALIZATION_NVP(tx);
    // note that the reset automagically maintains the reference count
    t.reset(tx);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class T>
inline void serialize(
    Archive & ar,
    std::unique_ptr< T > &t,
    const unsigned int file_version
){
    lslboost::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace lslboost


#endif // BOOST_SERIALIZATION_UNIQUE_PTR_HPP
