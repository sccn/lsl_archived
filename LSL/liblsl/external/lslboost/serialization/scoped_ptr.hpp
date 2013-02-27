#ifndef BOOST_SERIALIZATION_SCOPED_PTR_HPP_VP_2003_10_30
#define BOOST_SERIALIZATION_SCOPED_PTR_HPP_VP_2003_10_30

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2003 Vladimir Prus.
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)

// Provides non-intrusive serialization for lslboost::scoped_ptr
// Does not allow to serialize scoped_ptr's to builtin types.

#include <lslboost/config.hpp>

#include <lslboost/scoped_ptr.hpp>
#include <lslboost/serialization/nvp.hpp>
#include <lslboost/serialization/split_free.hpp>

namespace lslboost { 
namespace serialization {
    
    template<class Archive, class T>
    void save(
        Archive & ar, 
        const lslboost::scoped_ptr< T > & t, 
        const unsigned int /* version */
    ){
        T* r = t.get();
        ar << lslboost::serialization::make_nvp("scoped_ptr", r);
    }

    template<class Archive, class T>
    void load(
        Archive & ar, 
        lslboost::scoped_ptr< T > & t, 
        const unsigned int /* version */
    ){
        T* r;
        ar >> lslboost::serialization::make_nvp("scoped_ptr", r);
        t.reset(r); 
    }

    template<class Archive, class T>
    void serialize(
        Archive& ar, 
        lslboost::scoped_ptr< T >& t, 
        const unsigned int version
    ){
        lslboost::serialization::split_free(ar, t, version);
    }

} // namespace serialization
} // namespace lslboost

#endif // BOOST_SERIALIZATION_SCOPED_PTR_HPP_VP_2003_10_30
