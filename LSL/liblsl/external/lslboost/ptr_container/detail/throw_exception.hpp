//
// Boost.Pointer Container
//
//  Copyright Thorsten Ottosen 2006. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.lslboost.org/libs/ptr_container/
//

#ifndef BOOST_PTR_CONTAINER_DETAIL_THROW_EXCEPTION
#define BOOST_PTR_CONTAINER_DETAIL_THROW_EXCEPTION

#include <lslboost/assert.hpp>
#include <lslboost/config.hpp>

#ifdef BOOST_NO_EXCEPTIONS
#define BOOST_PTR_CONTAINER_NO_EXCEPTIONS
#endif

#ifdef BOOST_PTR_CONTAINER_NO_EXCEPTIONS

#define BOOST_PTR_CONTAINER_THROW_EXCEPTION( If, Ex, Msg ) BOOST_ASSERT( !(If) && Msg ) 

#else

#define BOOST_PTR_CONTAINER_THROW_EXCEPTION( If, Ex, Msg ) if( (If) ) throw Ex ( Msg )

#endif // BOOST_PTR_CONTAINER_NO_EXCEPTIONS


#endif
