#ifndef BOOST_SERIALIZATION_EPHEMERAL_HPP
#define BOOST_SERIALIZATION_EPHEMERAL_HPP

// MS compatible compilers support 
#pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// ephemeral_object.hpp: interface for serialization system.

// (C) Copyright 2007 Matthias Troyer. 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org for updates, documentation, and revision history.

#include <utility>

#include <lslboost/config.hpp>
#include <lslboost/detail/workaround.hpp>
// supress noise
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1200)
# pragma warning (disable : 4786) // too long name, harmless warning
#endif

#include <lslboost/mpl/integral_c.hpp>
#include <lslboost/mpl/integral_c_tag.hpp>

#include <lslboost/serialization/level.hpp>
#include <lslboost/serialization/tracking.hpp>
#include <lslboost/serialization/split_member.hpp>
#include <lslboost/serialization/base_object.hpp>
#include <lslboost/serialization/traits.hpp>
#include <lslboost/serialization/wrapper.hpp>

namespace lslboost {
namespace serialization {

template<class T>
struct ephemeral_object : 
    public wrapper_traits<ephemeral_object<T> >
{
    explicit ephemeral_object(T& t) :
        val(t)
    {}

    T & value() const {
        return val;
    }

    const T & const_value() const {
        return val;
    }

    template<class Archive>
    void serialize(Archive &ar, const unsigned int) const
    {
       ar & val;
    }

private:
    T & val;
};

template<class T>
inline
#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
const
#endif
ephemeral_object<T> ephemeral(const char * name, T & t){
    return ephemeral_object<T>(name, t);
}

} // seralization
} // lslboost

#endif // BOOST_SERIALIZATION_EPHEMERAL_HPP
