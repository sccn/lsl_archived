#ifndef  BOOST_SERIALIZATION_SET_HPP
#define BOOST_SERIALIZATION_SET_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// set.hpp: serialization for stl set templates

// (C) Copyright 2002-2014 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org for updates, documentation, and revision history.

#include <set>

#include <lslboost/config.hpp>

#include <lslboost/archive/detail/basic_iarchive.hpp>
#include <lslboost/serialization/access.hpp>
#include <lslboost/serialization/nvp.hpp>
#include <lslboost/serialization/detail/stack_constructor.hpp>
#include <lslboost/serialization/collection_size_type.hpp>
#include <lslboost/serialization/item_version_type.hpp>

#include <lslboost/serialization/collections_save_imp.hpp>
#include <lslboost/serialization/split_free.hpp>

namespace lslboost { 
namespace serialization {

template<class Archive, class Container>
inline void load_set_collection(Archive & ar, Container &s)
{
    s.clear();
    const lslboost::archive::library_version_type library_version(
        ar.get_library_version()
    );
    // retrieve number of elements
    item_version_type item_version(0);
    collection_size_type count;
    ar >> BOOST_SERIALIZATION_NVP(count);
    if(lslboost::archive::library_version_type(3) < library_version){
        ar >> BOOST_SERIALIZATION_NVP(item_version);
    }
    typename Container::iterator hint;
    hint = s.begin();
    while(count-- > 0){
        typedef typename Container::value_type type;
        detail::stack_construct<Archive, type> t(ar, item_version);
        // borland fails silently w/o full namespace
        ar >> lslboost::serialization::make_nvp("item", t.reference());
        typename Container::iterator result = s.insert(hint, t.reference());
        ar.reset_object_address(& (* result), & t.reference());
        hint = result;
    }
}

template<class Archive, class Key, class Compare, class Allocator >
inline void save(
    Archive & ar,
    const std::set<Key, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    lslboost::serialization::stl::save_collection<
        Archive, std::set<Key, Compare, Allocator> 
    >(ar, t);
}

template<class Archive, class Key, class Compare, class Allocator >
inline void load(
    Archive & ar,
    std::set<Key, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    load_set_collection(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Key, class Compare, class Allocator >
inline void serialize(
    Archive & ar,
    std::set<Key, Compare, Allocator> & t,
    const unsigned int file_version
){
    lslboost::serialization::split_free(ar, t, file_version);
}

// multiset
template<class Archive, class Key, class Compare, class Allocator >
inline void save(
    Archive & ar,
    const std::multiset<Key, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    lslboost::serialization::stl::save_collection<
        Archive, 
        std::multiset<Key, Compare, Allocator> 
    >(ar, t);
}

template<class Archive, class Key, class Compare, class Allocator >
inline void load(
    Archive & ar,
    std::multiset<Key, Compare, Allocator> &t,
    const unsigned int /* file_version */
){
    load_set_collection(ar, t);
}

// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
template<class Archive, class Key, class Compare, class Allocator >
inline void serialize(
    Archive & ar,
    std::multiset<Key, Compare, Allocator> & t,
    const unsigned int file_version
){
    lslboost::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace lslboost

#include <lslboost/serialization/collection_traits.hpp>

BOOST_SERIALIZATION_COLLECTION_TRAITS(std::set)
BOOST_SERIALIZATION_COLLECTION_TRAITS(std::multiset)

#endif // BOOST_SERIALIZATION_SET_HPP
