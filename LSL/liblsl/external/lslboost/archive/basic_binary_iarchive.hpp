#ifndef BOOST_ARCHIVE_BASIC_BINARY_IARCHIVE_HPP
#define BOOST_ARCHIVE_BASIC_BINARY_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_binary_iarchive.hpp
//
// archives stored as native binary - this should be the fastest way
// to archive the state of a group of obects.  It makes no attempt to
// convert to any canonical form.

// IN GENERAL, ARCHIVES CREATED WITH THIS CLASS WILL NOT BE READABLE
// ON PLATFORM APART FROM THE ONE THEY ARE CREATED ON

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org for updates, documentation, and revision history.

#include <lslboost/config.hpp>
#include <lslboost/detail/workaround.hpp>
#include <lslboost/serialization/pfto.hpp>

#include <lslboost/archive/basic_archive.hpp>
#include <lslboost/archive/detail/common_iarchive.hpp>
#include <lslboost/serialization/collection_size_type.hpp>
#include <lslboost/serialization/string.hpp>
#include <lslboost/serialization/item_version_type.hpp>
#include <lslboost/integer_traits.hpp>

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

#include <lslboost/archive/detail/abi_prefix.hpp> // must be the last header

namespace lslboost { 
namespace archive {

/////////////////////////////////////////////////////////////////////////
// class basic_binary_iarchive - read serialized objects from a input binary stream
template<class Archive>
class basic_binary_iarchive : 
    public detail::common_iarchive<Archive>
{
protected:
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1300)
public:
#elif defined(BOOST_MSVC)
    // for some inexplicable reason insertion of "class" generates compile erro
    // on msvc 7.1
    friend detail::interface_iarchive<Archive>;
#else
    friend class detail::interface_iarchive<Archive>;
#endif
    // intermediate level to support override of operators
    // fot templates in the absence of partial function 
    // template ordering. If we get here pass to base class
    // note extra nonsense to sneak it pass the borland compiers
    typedef detail::common_iarchive<Archive> detail_common_iarchive;
    template<class T>
    void load_override(T & t, BOOST_PFTO int version){
      this->detail_common_iarchive::load_override(t, static_cast<int>(version));
    }

    // include these to trap a change in binary format which
    // isn't specifically handled
    // upto 32K classes
    BOOST_STATIC_ASSERT(sizeof(class_id_type) == sizeof(int_least16_t));
    BOOST_STATIC_ASSERT(sizeof(class_id_reference_type) == sizeof(int_least16_t));
    // upto 2G objects
    BOOST_STATIC_ASSERT(sizeof(object_id_type) == sizeof(uint_least32_t));
    BOOST_STATIC_ASSERT(sizeof(object_reference_type) == sizeof(uint_least32_t));

    // binary files don't include the optional information 
    void load_override(class_id_optional_type & /* t */, int){}

    void load_override(tracking_type & t, int /*version*/){
        library_version_type lvt = this->get_library_version();
        if(lslboost::archive::library_version_type(6) < lvt){
            int_least8_t x=0;
            * this->This() >> x;
            t = lslboost::archive::tracking_type(x);
        }
        else{
            bool x=0;
            * this->This() >> x;
            t = lslboost::archive::tracking_type(x);
        }
    }
    void load_override(class_id_type & t, int version){
        library_version_type lvt = this->get_library_version();
        if(lslboost::archive::library_version_type(7) < lvt){
            this->detail_common_iarchive::load_override(t, version);
        }
        else
        if(lslboost::archive::library_version_type(6) < lvt){
            int_least16_t x=0;
            * this->This() >> x;
            t = lslboost::archive::class_id_type(x);
        }
        else{
            int x=0;
            * this->This() >> x;
            t = lslboost::archive::class_id_type(x);
        }
    }
    void load_override(class_id_reference_type & t, int version){
        load_override(static_cast<class_id_type &>(t), version);
    }
#if 0
    void load_override(class_id_reference_type & t, int version){
        library_version_type lvt = this->get_library_version();
        if(lslboost::archive::library_version_type(7) < lvt){
            this->detail_common_iarchive::load_override(t, version);
        }
        else
        if(lslboost::archive::library_version_type(6) < lvt){
            int_least16_t x=0;
            * this->This() >> x;
            t = lslboost::archive::class_id_reference_type(
                lslboost::archive::class_id_type(x)
            );
        }
        else{
            int x=0;
            * this->This() >> x;
            t = lslboost::archive::class_id_reference_type(
                lslboost::archive::class_id_type(x)
            );
        }
    }
#endif

    void load_override(version_type & t, int version){
        library_version_type lvt = this->get_library_version();
        if(lslboost::archive::library_version_type(7) < lvt){
            this->detail_common_iarchive::load_override(t, version);
        }
        else
        if(lslboost::archive::library_version_type(6) < lvt){
            uint_least8_t x=0;
            * this->This() >> x;
            t = lslboost::archive::version_type(x);
        }
        else
        if(lslboost::archive::library_version_type(5) < lvt){
            uint_least16_t x=0;
            * this->This() >> x;
            t = lslboost::archive::version_type(x);
        }
        else
        if(lslboost::archive::library_version_type(2) < lvt){
            // upto 255 versions
            unsigned char x=0;
            * this->This() >> x;
            t = version_type(x);
        }
        else{
            unsigned int x=0;
            * this->This() >> x;
            t = lslboost::archive::version_type(x);
        }
    }

    void load_override(lslboost::serialization::item_version_type & t, int version){
        library_version_type lvt = this->get_library_version();
//        if(lslboost::archive::library_version_type(7) < lvt){
        if(lslboost::archive::library_version_type(6) < lvt){
            this->detail_common_iarchive::load_override(t, version);
        }
        else
        if(lslboost::archive::library_version_type(6) < lvt){
            uint_least16_t x=0;
            * this->This() >> x;
            t = lslboost::serialization::item_version_type(x);
        }
        else{
            unsigned int x=0;
            * this->This() >> x;
            t = lslboost::serialization::item_version_type(x);
        }
    }

    void load_override(serialization::collection_size_type & t, int version){
        if(lslboost::archive::library_version_type(5) < this->get_library_version()){
            this->detail_common_iarchive::load_override(t, version);
        }
        else{
            unsigned int x=0;
            * this->This() >> x;
            t = serialization::collection_size_type(x);
        } 
    }

    BOOST_ARCHIVE_OR_WARCHIVE_DECL(void)
    load_override(class_name_type & t, int);
    BOOST_ARCHIVE_OR_WARCHIVE_DECL(void)
    init();
   
    basic_binary_iarchive(unsigned int flags) :
        detail::common_iarchive<Archive>(flags)
    {}
};

} // namespace archive
} // namespace lslboost

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#include <lslboost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // BOOST_ARCHIVE_BASIC_BINARY_IARCHIVE_HPP
