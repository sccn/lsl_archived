#ifndef BOOST_ARCHIVE_SHARED_PTR_HELPER_HPP
#define BOOST_ARCHIVE_SHARED_PTR_HELPER_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// shared_ptr_helper.hpp: serialization for lslboost shared pointern

// (C) Copyright 2004-2009 Robert Ramey, Martin Ecker and Takatoshi Kondo
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org for updates, documentation, and revision history.

#include <set>
#include <list>
#include <utility>
#include <cstddef> // NULL

#include <lslboost/config.hpp>
#include <lslboost/shared_ptr.hpp>

#include <lslboost/type_traits/is_polymorphic.hpp>
#include <lslboost/serialization/type_info_implementation.hpp>
#include <lslboost/serialization/shared_ptr_132.hpp>
#include <lslboost/serialization/throw_exception.hpp>

#include <lslboost/archive/archive_exception.hpp>
#include <lslboost/archive/detail/decl.hpp>

#include <lslboost/archive/detail/abi_prefix.hpp> // must be the last headern

namespace lslboost_132 {
    template<class T> class shared_ptr;
}
namespace lslboost {
    template<class T> class shared_ptr;
    namespace serialization {
        class extended_type_info;
        template<class Archive, class T>
        inline void load(
            Archive & ar,
            lslboost::shared_ptr< T > &t,
            const unsigned int file_version
        );
    }
namespace archive{
namespace detail {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// a common class for holding various types of shared pointers

class shared_ptr_helper {
    struct collection_type_compare {
        bool operator()(
            const shared_ptr<const void> &lhs,
            const shared_ptr<const void> &rhs
        )const{
            return lhs.get() < rhs.get();
        }
    };
    typedef std::set<
        lslboost::shared_ptr<const void>,
        collection_type_compare
    > collection_type;
    typedef collection_type::const_iterator iterator_type;
    // list of shared_pointers create accessable by raw pointer. This
    // is used to "match up" shared pointers loaded at different
    // points in the archive. Note, we delay construction until
    // it is actually used since this is by default included as
    // a "mix-in" even if shared_ptr isn't used.
    collection_type * m_pointers;

    struct null_deleter {
        void operator()(void const *) const {}
    };

    struct void_deleter {
        const lslboost::serialization::extended_type_info * m_eti;
        void_deleter(const lslboost::serialization::extended_type_info *eti) :
            m_eti(eti)
        {}
        void operator()(void *vp) const {
            m_eti->destroy(vp);
        }
    };

#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
    template<class Archive, class T>
    friend inline void lslboost::serialization::load(
        Archive & ar,
        lslboost::shared_ptr< T > &t,
        const unsigned int file_version
    );
#endif

//  #ifdef BOOST_SERIALIZATION_SHARED_PTR_132_HPP
    // list of loaded pointers.  This is used to be sure that the pointers
    // stay around long enough to be "matched" with other pointers loaded
    // by the same archive.  These are created with a "null_deleter" so that
    // when this list is destroyed - the underlaying raw pointers are not
    // destroyed.  This has to be done because the pointers are also held by
    // new system which is disjoint from this set.  This is implemented
    // by a change in load_construct_data below.  It makes this file suitable
    // only for loading pointers into a 1.33 or later lslboost system.
    std::list<lslboost_132::shared_ptr<const void> > * m_pointers_132;
//  #endif

    // returns pointer to object and an indicator whether this is a
    // new entry (true) or a previous one (false)
    BOOST_ARCHIVE_DECL(shared_ptr<void>) 
    get_od(
        const void * od,
        const lslboost::serialization::extended_type_info * true_type, 
        const lslboost::serialization::extended_type_info * this_type
    );

    BOOST_ARCHIVE_DECL(void)
    append(const lslboost::shared_ptr<const void> &);

    template<class T>
    struct non_polymorphic {
        static const lslboost::serialization::extended_type_info * 
        get_object_identifier(T &){
            return & lslboost::serialization::singleton<
                BOOST_DEDUCED_TYPENAME 
                lslboost::serialization::type_info_implementation< T >::type
            >::get_const_instance();
        }
    };
    template<class T>
    struct polymorphic {
        static const lslboost::serialization::extended_type_info * 
        get_object_identifier(T & t){
            return lslboost::serialization::singleton<
                BOOST_DEDUCED_TYPENAME 
                lslboost::serialization::type_info_implementation< T >::type
            >::get_const_instance().get_derived_extended_type_info(t);
        }
    };
public:
    template<class T>
    void reset(shared_ptr< T > & s, T * t){
        if(NULL == t){
            s.reset();
            return;
        }
        const lslboost::serialization::extended_type_info * this_type
            = & lslboost::serialization::type_info_implementation< T >::type
                    ::get_const_instance();

        // get pointer to the most derived object.  This is effectively
        // the object identifern
        typedef BOOST_DEDUCED_TYPENAME mpl::eval_if<
            is_polymorphic< T >,
            mpl::identity<polymorphic< T > >,
            mpl::identity<non_polymorphic< T > >
        >::type type;

        const lslboost::serialization::extended_type_info * true_type
            = type::get_object_identifier(*t);

        // note:if this exception is thrown, be sure that derived pointern
        // is either registered or exported.
        if(NULL == true_type)
            lslboost::serialization::throw_exception(
                archive_exception(
                    archive_exception::unregistered_class,
                    this_type->get_debug_info()
                )
            );
        shared_ptr<void> r =
            get_od(
                static_cast<const void *>(t), 
                true_type,
                this_type
            );
        if(!r){
            s.reset(t);
            const void * od = void_downcast(
                *true_type,
                *this_type,
                static_cast<const void *>(t)
            );
            shared_ptr<const void> sp(s, od);
            append(sp);
        }
        else{
            s = shared_ptr< T >(
                r,
                static_cast<T *>(r.get())
            );
        }
    }

//  #ifdef BOOST_SERIALIZATION_SHARED_PTR_132_HPP
    BOOST_ARCHIVE_DECL(void)
    append(const lslboost_132::shared_ptr<const void> & t);
//  #endif
public:
    BOOST_ARCHIVE_DECL(BOOST_PP_EMPTY())
    shared_ptr_helper();
    BOOST_ARCHIVE_DECL(BOOST_PP_EMPTY())
    ~shared_ptr_helper();
};

} // namespace detail
} // namespace archive
} // namespace lslboost

#include <lslboost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // BOOST_ARCHIVE_SHARED_PTR_HELPER_HPP
