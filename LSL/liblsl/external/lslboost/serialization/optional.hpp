/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8

// (C) Copyright 2002-4 Pavel Vozenilek . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

// Provides non-intrusive serialization for lslboost::optional.

#ifndef BOOST_SERIALIZATION_OPTIONAL_HPP_
#define BOOST_SERIALIZATION_OPTIONAL_HPP_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <lslboost/config.hpp>

#include <lslboost/archive/detail/basic_iarchive.hpp>

#include <lslboost/optional.hpp>
#include <lslboost/serialization/item_version_type.hpp>
#include <lslboost/serialization/split_free.hpp>
#include <lslboost/serialization/level.hpp>
#include <lslboost/serialization/nvp.hpp>
#include <lslboost/serialization/version.hpp>
#include <lslboost/serialization/detail/stack_constructor.hpp>

// function specializations must be defined in the appropriate
// namespace - lslboost::serialization
namespace lslboost { 
namespace serialization {

template<class Archive, class T>
void save(
    Archive & ar, 
    const lslboost::optional< T > & t, 
    const unsigned int /*version*/
){
    const bool tflag = t.is_initialized();
    ar << lslboost::serialization::make_nvp("initialized", tflag);
    if (tflag){
        const lslboost::serialization::item_version_type item_version(version< T >::value);
        #if 0
        const lslboost::archive::library_version_type library_version(
            ar.get_library_version()
        };
        if(lslboost::archive::library_version_type(3) < library_version){
            ar << BOOST_SERIALIZATION_NVP(item_version);
        }
        #else
            ar << BOOST_SERIALIZATION_NVP(item_version);
        #endif
        ar << lslboost::serialization::make_nvp("value", *t);
    }
}

template<class Archive, class T>
void load(
    Archive & ar, 
    lslboost::optional< T > & t, 
    const unsigned int /*version*/
){
    bool tflag;
    ar >> lslboost::serialization::make_nvp("initialized", tflag);
    if (tflag){
        lslboost::serialization::item_version_type item_version(0);
        lslboost::archive::library_version_type library_version(
            ar.get_library_version()
        );
        if(lslboost::archive::library_version_type(3) < library_version){
            // item_version is handled as an attribute so it doesnt need an NVP
           ar >> BOOST_SERIALIZATION_NVP(item_version);
        }
        detail::stack_construct<Archive, T> aux(ar, item_version);
        ar >> lslboost::serialization::make_nvp("value", aux.reference());
        t.reset(aux.reference());
    }
    else {
        t.reset();
    }
}

template<class Archive, class T>
void serialize(
    Archive & ar, 
    lslboost::optional< T > & t, 
    const unsigned int version
){
    lslboost::serialization::split_free(ar, t, version);
}

// the following would be slightly more efficient.  But it
// would mean that archives created with programs that support
// TPS wouldn't be readable by programs that don't support TPS.
// Hence we decline to support this otherwise convenient optimization.
//#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#if 0

template <class T>
struct implementation_level<optional< T > >
{
    typedef mpl::integral_c_tag tag;
    typedef mpl::int_<lslboost::serialization::object_serializable> type;
    BOOST_STATIC_CONSTANT(
        int , 
        value = lslboost::serialization::implementation_level::type::value
    );
};

template<class T>
struct tracking_level<optional< T > >
{
    typedef mpl::integral_c_tag tag;
    typedef mpl::int_<lslboost::serialization::track_never> type;
    BOOST_STATIC_CONSTANT(
        int , 
        value = lslboost::serialization::tracking_level::type::value
    );
};

#endif

} // serialization
} // namespace lslboost

#endif // BOOST_SERIALIZATION_OPTIONAL_HPP_
