#ifndef BOOST_ARCHIVE_ITERATORS_ESCAPE_HPP
#define BOOST_ARCHIVE_ITERATORS_ESCAPE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// escape.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org for updates, documentation, and revision history.

#include <lslboost/assert.hpp>
#include <cstddef> // NULL

#include <lslboost/config.hpp> // for BOOST_DEDUCED_TYPENAME
#include <lslboost/iterator/iterator_adaptor.hpp>
#include <lslboost/iterator/iterator_traits.hpp>

namespace lslboost { 
namespace archive {
namespace iterators {

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// insert escapes into text

template<class Derived, class Base>
class escape : 
    public lslboost::iterator_adaptor<
        Derived, 
        Base, 
        BOOST_DEDUCED_TYPENAME lslboost::iterator_value<Base>::type,
        single_pass_traversal_tag,
        BOOST_DEDUCED_TYPENAME lslboost::iterator_value<Base>::type
    >
{
    typedef BOOST_DEDUCED_TYPENAME lslboost::iterator_value<Base>::type base_value_type;
    typedef BOOST_DEDUCED_TYPENAME lslboost::iterator_reference<Base>::type reference_type;
    friend class lslboost::iterator_core_access;

    typedef BOOST_DEDUCED_TYPENAME lslboost::iterator_adaptor<
        Derived, 
        Base, 
        base_value_type,
        single_pass_traversal_tag,
        base_value_type
    > super_t;

    typedef escape<Derived, Base> this_t;

    void dereference_impl() {
        m_current_value = static_cast<Derived *>(this)->fill(m_bnext, m_bend);
        m_full = true;
    }

    //Access the value referred to 
    reference_type dereference() const {
        if(!m_full)
            const_cast<this_t *>(this)->dereference_impl();
        return m_current_value;
    }

    bool equal(const this_t & rhs) const {
        if(m_full){
            if(! rhs.m_full)
                const_cast<this_t *>(& rhs)->dereference_impl();
        }
        else{
            if(rhs.m_full)
                const_cast<this_t *>(this)->dereference_impl();
        }
        if(m_bnext != rhs.m_bnext)
            return false;
        if(this->base_reference() != rhs.base_reference())
            return false;
        return true;
    }

   void increment(){
        if(++m_bnext < m_bend){
            m_current_value = *m_bnext;
            return;
        }
        ++(this->base_reference());
        m_bnext = NULL;
        m_bend = NULL;
        m_full = false;
    }

    // buffer to handle pending characters
    const base_value_type *m_bnext;
    const base_value_type *m_bend;
    bool m_full;
    base_value_type m_current_value;
public:
    escape(Base base) : 
        super_t(base),
        m_bnext(NULL),
        m_bend(NULL),
        m_full(false)
    {
    }
};

} // namespace iterators
} // namespace archive
} // namespace lslboost

#endif // BOOST_ARCHIVE_ITERATORS_ESCAPE_HPP
