#ifndef BOOST_INTERPROCESS_DETAIL_SP_COUNTED_BASE_ATOMIC_HPP_INCLUDED
#define BOOST_INTERPROCESS_DETAIL_SP_COUNTED_BASE_ATOMIC_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright (c) 2001, 2002, 2003 Peter Dimov and Multi Media Ltd.
//  Copyright 2004-2005 Peter Dimov
//  Copyright 2007-2011 Ion Gaztanaga
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
//
//  Lock-free algorithm by Alexander Terekhov
//
//  Thanks to Ben Hitchings for the #weak + (#shared != 0)
//  formulation
//

#include <lslboost/interprocess/detail/config_begin.hpp>
#include <lslboost/interprocess/detail/workaround.hpp>

#include <lslboost/interprocess/detail/atomic.hpp>
#include <typeinfo>

namespace lslboost {

namespace interprocess {

namespace ipcdetail {

class sp_counted_base
{
private:

    sp_counted_base( sp_counted_base const & );
    sp_counted_base & operator= ( sp_counted_base const & );

    lslboost::uint32_t use_count_;        // #shared
    lslboost::uint32_t weak_count_;       // #weak + (#shared != 0)

public:

    sp_counted_base(): use_count_( 1 ), weak_count_( 1 )
    {}

    ~sp_counted_base() // nothrow
    {}

    void add_ref_copy()
    {
        ipcdetail::atomic_inc32( &use_count_ );
    }

    bool add_ref_lock() // true on success
    {
        for( ;; )
        {
            lslboost::uint32_t tmp = static_cast< lslboost::uint32_t const volatile& >( use_count_ );
            if( tmp == 0 ) return false;
            if( ipcdetail::atomic_cas32( &use_count_, tmp + 1, tmp ) == tmp )
               return true;
        }
    }

   bool ref_release() // nothrow
   { return 1 == ipcdetail::atomic_dec32( &use_count_ );  }

   void weak_add_ref() // nothrow
   { ipcdetail::atomic_inc32( &weak_count_ ); }

   bool weak_release() // nothrow
   { return 1 == ipcdetail::atomic_dec32( &weak_count_ ); }

   long use_count() const // nothrow
   { return (long)static_cast<lslboost::uint32_t const volatile &>( use_count_ ); }
};

} // namespace ipcdetail

} // namespace interprocess

} // namespace lslboost

#include <lslboost/interprocess/detail/config_end.hpp>

#endif  // #ifndef BOOST_INTERPROCESS_DETAIL_SP_COUNTED_BASE_ATOMIC_HPP_INCLUDED
