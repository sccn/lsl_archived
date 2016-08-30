#ifndef BOOST_ARCHIVE_XML_IARCHIVE_HPP
#define BOOST_ARCHIVE_XML_IARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// xml_iarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org for updates, documentation, and revision history.

#include <istream>

//#include <lslboost/scoped_ptr.hpp>
#include <lslboost/archive/detail/auto_link_archive.hpp>
#include <lslboost/archive/basic_text_iprimitive.hpp>
#include <lslboost/archive/basic_xml_iarchive.hpp>
#include <lslboost/archive/detail/register_archive.hpp>
#include <lslboost/serialization/item_version_type.hpp>

#include <lslboost/archive/detail/abi_prefix.hpp> // must be the last header

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace lslboost { 
namespace archive {

template<class CharType>
class basic_xml_grammar;
typedef basic_xml_grammar<char> xml_grammar;

template<class Archive>
class xml_iarchive_impl : 
    public basic_text_iprimitive<std::istream>,
    public basic_xml_iarchive<Archive>
{
#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
public:
#else
    friend class detail::interface_iarchive<Archive>;
    friend class basic_xml_iarchive<Archive>;
    friend class load_access;
protected:
#endif
    // instances of micro xml parser to parse start preambles
    // scoped_ptr doesn't play nice with borland - so use a naked pointer
    // scoped_ptr<xml_grammar> gimpl;
    xml_grammar *gimpl;

    std::istream & get_is(){
        return is;
    }
    template<class T>
    void load(T & t){
        basic_text_iprimitive<std::istream>::load(t);
    }
    void 
    load(version_type & t){
        unsigned int v;
        load(v);
        t = version_type(v);
    }
    void 
    load(lslboost::serialization::item_version_type & t){
        unsigned int v;
        load(v);
        t = lslboost::serialization::item_version_type(v);
    }
    BOOST_ARCHIVE_DECL(void)
    load(char * t);
    #ifndef BOOST_NO_INTRINSIC_WCHAR_T
    BOOST_ARCHIVE_DECL(void)
    load(wchar_t * t);
    #endif
    BOOST_ARCHIVE_DECL(void)
    load(std::string &s);
    #ifndef BOOST_NO_STD_WSTRING
    BOOST_ARCHIVE_DECL(void)
    load(std::wstring &ws);
    #endif
    template<class T>
    void load_override(T & t, BOOST_PFTO int){
        basic_xml_iarchive<Archive>::load_override(t, 0);
    }
    BOOST_ARCHIVE_DECL(void)
    load_override(class_name_type & t, int);
    BOOST_ARCHIVE_DECL(void)
    init();
    BOOST_ARCHIVE_DECL(BOOST_PP_EMPTY()) 
    xml_iarchive_impl(std::istream & is, unsigned int flags);
    BOOST_ARCHIVE_DECL(BOOST_PP_EMPTY())
    ~xml_iarchive_impl();
};

// do not derive from the classes below.  If you want to extend this functionality
// via inhertance, derived from text_iarchive_impl instead.  This will
// preserve correct static polymorphism.

// same as xml_iarchive below - without the shared_ptr_helper
class naked_xml_iarchive : 
    public xml_iarchive_impl<naked_xml_iarchive>
{
public:
    naked_xml_iarchive(std::istream & is, unsigned int flags = 0) :
        xml_iarchive_impl<naked_xml_iarchive>(is, flags)
    {}
    ~naked_xml_iarchive(){}
};

} // namespace archive
} // namespace lslboost

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#include <lslboost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

// note special treatment of shared_ptr. This type needs a special
// structure associated with every archive.  We created a "mix-in"
// class to provide this functionality.  Since shared_ptr holds a
// special esteem in the lslboost library - we included it here by default.
#include <lslboost/archive/shared_ptr_helper.hpp>

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace lslboost { 
namespace archive {

class xml_iarchive : 
    public xml_iarchive_impl<xml_iarchive>,
    public detail::shared_ptr_helper
{
public:
    xml_iarchive(std::istream & is, unsigned int flags = 0) :
        xml_iarchive_impl<xml_iarchive>(is, flags)
    {}
    ~xml_iarchive(){};
};

} // namespace archive
} // namespace lslboost

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(lslboost::archive::xml_iarchive)

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif // BOOST_ARCHIVE_XML_IARCHIVE_HPP
