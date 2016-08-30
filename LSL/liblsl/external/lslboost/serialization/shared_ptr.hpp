#ifndef BOOST_SERIALIZATION_SHARED_PTR_HPP
#define BOOST_SERIALIZATION_SHARED_PTR_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// shared_ptr.hpp: serialization for lslboost shared pointer

// (C) Copyright 2004 Robert Ramey and Martin Ecker
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org for updates, documentation, and revision history.

#include <cstddef> // NULL

#include <lslboost/config.hpp>
#include <lslboost/mpl/integral_c.hpp>
#include <lslboost/mpl/integral_c_tag.hpp>

#include <lslboost/detail/workaround.hpp>
#include <lslboost/shared_ptr.hpp>

#include <lslboost/serialization/split_free.hpp>
#include <lslboost/serialization/nvp.hpp>
#include <lslboost/serialization/version.hpp>
#include <lslboost/serialization/tracking.hpp>

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// shared_ptr serialization traits
// version 1 to distinguish from lslboost 1.32 version. Note: we can only do this
// for a template when the compiler supports partial template specialization

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    namespace lslboost {
    namespace serialization{
        template<class T>
        struct version< ::lslboost::shared_ptr< T > > {
            typedef mpl::integral_c_tag tag;
            #if BOOST_WORKAROUND(__MWERKS__, BOOST_TESTED_AT(0x3206))
            typedef BOOST_DEDUCED_TYPENAME mpl::int_<1> type;
            #else
            typedef mpl::int_<1> type;
            #endif
            #if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x570))
            BOOST_STATIC_CONSTANT(int, value = 1);
            #else
            BOOST_STATIC_CONSTANT(int, value = type::value);
            #endif
        };
        // don't track shared pointers
        template<class T>
        struct tracking_level< ::lslboost::shared_ptr< T > > { 
            typedef mpl::integral_c_tag tag;
            #if BOOST_WORKAROUND(__MWERKS__, BOOST_TESTED_AT(0x3206))
            typedef BOOST_DEDUCED_TYPENAME mpl::int_< ::lslboost::serialization::track_never> type;
            #else
            typedef mpl::int_< ::lslboost::serialization::track_never> type;
            #endif
            #if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x570))
            BOOST_STATIC_CONSTANT(int, value = ::lslboost::serialization::track_never);
            #else
            BOOST_STATIC_CONSTANT(int, value = type::value);
            #endif
        };
    }}
    #define BOOST_SERIALIZATION_SHARED_PTR(T)
#else
    // define macro to let users of these compilers do this
    #define BOOST_SERIALIZATION_SHARED_PTR(T)                         \
    BOOST_CLASS_VERSION(                                              \
        ::lslboost::shared_ptr< T >,                                     \
        1                                                             \
    )                                                                 \
    BOOST_CLASS_TRACKING(                                             \
        ::lslboost::shared_ptr< T >,                                     \
        ::lslboost::serialization::track_never                           \
    )                                                                 \
    /**/
#endif

namespace lslboost {
namespace serialization{

struct null_deleter {
    void operator()(void const *) const {}
};

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// serialization for shared_ptr

template<class Archive, class T>
inline void save(
    Archive & ar,
    const lslboost::shared_ptr< T > &t,
    const unsigned int /* file_version */
){
    // The most common cause of trapping here would be serializing
    // something like shared_ptr<int>.  This occurs because int
    // is never tracked by default.  Wrap int in a trackable type
    BOOST_STATIC_ASSERT((tracking_level< T >::value != track_never));
    const T * t_ptr = t.get();
    ar << lslboost::serialization::make_nvp("px", t_ptr);
}

#ifdef BOOST_SERIALIZATION_SHARED_PTR_132_HPP
template<class Archive, class T>
inline void load(
    Archive & ar,
    lslboost::shared_ptr< T > &t,
    const unsigned int file_version
){
    // The most common cause of trapping here would be serializing
    // something like shared_ptr<int>.  This occurs because int
    // is never tracked by default.  Wrap int in a trackable type
    BOOST_STATIC_ASSERT((tracking_level< T >::value != track_never));
    T* r;
    if(file_version < 1){
        //ar.register_type(static_cast<
        //    lslboost_132::detail::sp_counted_base_impl<T *, lslboost::checked_deleter< T > > *
        //>(NULL));
        ar.register_type(static_cast<
            lslboost_132::detail::sp_counted_base_impl<T *, null_deleter > *
        >(NULL));
        lslboost_132::shared_ptr< T > sp;
        ar >> lslboost::serialization::make_nvp("px", sp.px);
        ar >> lslboost::serialization::make_nvp("pn", sp.pn);
        // got to keep the sps around so the sp.pns don't disappear
        ar.append(sp);
        r = sp.get();
    }
    else{
        ar >> lslboost::serialization::make_nvp("px", r);
    }
    ar.reset(t,r);
}

#else
template<class Archive, class T>
inline void load(
    Archive & ar,
    lslboost::shared_ptr< T > &t,
    const unsigned int /*file_version*/
){
    // The most common cause of trapping here would be serializing
    // something like shared_ptr<int>.  This occurs because int
    // is never tracked by default.  Wrap int in a trackable type
    BOOST_STATIC_ASSERT((tracking_level< T >::value != track_never));
    T* r;
    ar >> lslboost::serialization::make_nvp("px", r);
    ar.reset(t,r);
}
#endif

template<class Archive, class T>
inline void serialize(
    Archive & ar,
    lslboost::shared_ptr< T > &t,
    const unsigned int file_version
){
    // correct shared_ptr serialization depends upon object tracking
    // being used.
    BOOST_STATIC_ASSERT(
        lslboost::serialization::tracking_level< T >::value
        != lslboost::serialization::track_never
    );
    lslboost::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace lslboost

#endif // BOOST_SERIALIZATION_SHARED_PTR_HPP
