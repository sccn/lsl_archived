#ifndef BOOST_SMART_PTR_HPP_INCLUDED
#define BOOST_SMART_PTR_HPP_INCLUDED

//
//  smart_ptr.hpp
//
//  For convenience, this header includes the rest of the smart
//  pointer library headers.
//
//  Copyright (c) 2003 Peter Dimov  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
//  http://www.lslboost.org/libs/smart_ptr/smart_ptr.htm
//

#include <lslboost/config.hpp>

#include <lslboost/scoped_ptr.hpp>
#include <lslboost/scoped_array.hpp>
#include <lslboost/shared_ptr.hpp>
#include <lslboost/shared_array.hpp>

#if !defined(BOOST_NO_MEMBER_TEMPLATES) || defined(BOOST_MSVC6_MEMBER_TEMPLATES)
# include <lslboost/weak_ptr.hpp>
# include <lslboost/intrusive_ptr.hpp>
# include <lslboost/enable_shared_from_this.hpp>
# include <lslboost/make_shared.hpp>
#endif

#endif // #ifndef BOOST_SMART_PTR_HPP_INCLUDED
