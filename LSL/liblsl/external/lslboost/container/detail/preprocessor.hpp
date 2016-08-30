//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINER_DETAIL_PREPROCESSOR_HPP
#define BOOST_CONTAINER_DETAIL_PREPROCESSOR_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include <lslboost/container/detail/config_begin.hpp>
#include <lslboost/container/detail/workaround.hpp>
#include <lslboost/move/utility.hpp>

#ifdef BOOST_CONTAINER_PERFECT_FORWARDING
//#error "This file is not needed when perfect forwarding is available"
#endif   //BOOST_CONTAINER_PERFECT_FORWARDING

#include <lslboost/preprocessor/iteration/local.hpp>
#include <lslboost/preprocessor/punctuation/paren_if.hpp>
#include <lslboost/preprocessor/punctuation/comma_if.hpp>
#include <lslboost/preprocessor/control/expr_if.hpp>
#include <lslboost/preprocessor/cat.hpp>
#include <lslboost/preprocessor/repetition/enum.hpp>
#include <lslboost/preprocessor/repetition/enum_params.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing_params.hpp>
#include <lslboost/preprocessor/repetition/enum_trailing.hpp>
#include <lslboost/preprocessor/repetition/enum_shifted_params.hpp>
#include <lslboost/preprocessor/repetition/enum_shifted.hpp>
#include <lslboost/preprocessor/repetition/repeat.hpp>
#include <lslboost/preprocessor/logical/not.hpp>
#include <lslboost/preprocessor/arithmetic/sub.hpp>
#include <lslboost/preprocessor/arithmetic/add.hpp>
#include <lslboost/preprocessor/iteration/iterate.hpp>
#include <lslboost/move/utility.hpp>

#define BOOST_CONTAINER_MAX_CONSTRUCTOR_PARAMETERS 10

//Note:
//We define template parameters as const references to
//be able to bind temporaries. After that we will un-const them.
//This cast is ugly but it is necessary until "perfect forwarding"
//is achieved in C++0x. Meanwhile, if we want to be able to
//bind rvalues with non-const references, we have to be ugly
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   #define BOOST_CONTAINER_PP_PARAM_LIST(z, n, data) \
   BOOST_PP_CAT(P, n) && BOOST_PP_CAT(p, n) \
   //!
#else
   #define BOOST_CONTAINER_PP_PARAM_LIST(z, n, data) \
   const BOOST_PP_CAT(P, n) & BOOST_PP_CAT(p, n) \
   //!
#endif   //#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

#define BOOST_CONTAINER_PP_CONST_REF_PARAM_LIST_Q(z, n, Data) \
const BOOST_PP_CAT(Q, n) & BOOST_PP_CAT(q, n) \
//!

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   #define BOOST_CONTAINER_PP_PARAM(U, u) \
   U && u \
   //!
#else
   #define BOOST_CONTAINER_PP_PARAM(U, u) \
   const U & u \
   //!
#endif   //#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

   #define BOOST_CONTAINER_PP_PARAM_INIT(z, n, data) \
   BOOST_PP_CAT(m_p, n) (::lslboost::forward< BOOST_PP_CAT(P, n) >( BOOST_PP_CAT(p, n) ))           \
   //!

#else //BOOST_NO_CXX11_RVALUE_REFERENCES

   #define BOOST_CONTAINER_PP_PARAM_INIT(z, n, data) \
   BOOST_PP_CAT(m_p, n) (const_cast<BOOST_PP_CAT(P, n) &>(BOOST_PP_CAT(p, n))) \
   //!
#endif   //#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

   #if defined(BOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG)

      namespace lslboost {
      namespace container {
      namespace container_detail {
         template<class T>
         struct ref_holder;

         template<class T>
         struct ref_holder<T &>
         {
            explicit ref_holder(T &t)
               : t_(t)
            {}
            T &t_;
            T & get() {  return t_;   }
         };

         template<class T>
         struct ref_holder<const T>
         {
            explicit ref_holder(const T &t)
               : t_(t)
            {}
            const T &t_;
            const T & get() {  return t_;   }
         };

         template<class T>
         struct ref_holder<const T &&>
         {
            explicit ref_holder(const T &t)
               : t_(t)
            {}
            const T &t_;
            const T & get() {  return t_;   }
         };

         template<class T>
         struct ref_holder
         {
            explicit ref_holder(T &&t)
               : t_(t)
            {}
            T &t_;
            T && get() {  return ::lslboost::move(t_);   }
         };

         template<class T>
         struct ref_holder<T &&>
         {
            explicit ref_holder(T &&t)
               : t_(t)
            {}
            T &t_;
            T && get()  { return ::lslboost::move(t_); }
         };

      }  //namespace container_detail {
      }  //namespace container {
      }  //namespace lslboost {

      #define BOOST_CONTAINER_PP_PARAM_DEFINE(z, n, data)  \
         ::lslboost::container::container_detail::ref_holder<BOOST_PP_CAT(P, n)> BOOST_PP_CAT(m_p, n);  \
      //!

   #else //BOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG

      #define BOOST_CONTAINER_PP_PARAM_DEFINE(z, n, data)  \
      BOOST_PP_CAT(P, n) && BOOST_PP_CAT(m_p, n);            \
      //!

   #endif //defined(BOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG)

#else //BOOST_NO_CXX11_RVALUE_REFERENCES

   #define BOOST_CONTAINER_PP_PARAM_DEFINE(z, n, data)  \
   BOOST_PP_CAT(P, n) & BOOST_PP_CAT(m_p, n);             \
   //!
#endif   //#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && defined(BOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG)

   #define BOOST_CONTAINER_PP_MEMBER_FORWARD(z, n, data) BOOST_PP_CAT(this->m_p, n).get() \
   //!

#else //!defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && defined(BOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG)

   #define BOOST_CONTAINER_PP_MEMBER_FORWARD(z, n, data) \
   ::lslboost::forward< BOOST_PP_CAT(P, n) >( BOOST_PP_CAT(this->m_p, n) ) \
   //!

#endif   //!defined(BOOST_NO_CXX11_RVALUE_REFERENCES) && defined(BOOST_MOVE_MSVC_10_MEMBER_RVALUE_REF_BUG)

#define BOOST_CONTAINER_PP_PARAM_INC(z, n, data)   \
  BOOST_PP_CAT(++this->m_p, n)                     \
//!

#define BOOST_CONTAINER_PP_IDENTITY(z, n, data) data


#define BOOST_CONTAINER_PP_PARAM_FORWARD(z, n, data) \
::lslboost::forward< BOOST_PP_CAT(P, n) >( BOOST_PP_CAT(p, n) ) \
//!

#define BOOST_CONTAINER_PP_DECLVAL(z, n, data) \
::lslboost::move_detail::declval< BOOST_PP_CAT(P, n) >() \
//!

#define BOOST_CONTAINER_PP_MEMBER_IT_FORWARD(z, n, data) \
BOOST_PP_CAT(*this->m_p, n) \
//!

#define BOOST_CONTAINER_PP_TEMPLATE_PARAM_VOID_DEFAULT(z, n, data)   \
  BOOST_PP_CAT(class P, n) = void                                    \
//!

#define BOOST_CONTAINER_PP_TEMPLATE_PARAM_WITH_DEFAULT(z, n, default_type) \
  BOOST_PP_CAT(class P, n) = default_type                                  \
//!

#define BOOST_CONTAINER_PP_STATIC_PARAM_REF_DECLARE(z, n, data) \
   static BOOST_PP_CAT(P, n) & BOOST_PP_CAT(p, n); \
//!

#define BOOST_CONTAINER_PP_PARAM_PASS(z, n, data) \
   BOOST_PP_CAT(p, n) \
//!

#define BOOST_CONTAINER_PP_FWD_TYPE(z, n, data) \
   typename ::lslboost::move_detail::forward_type< BOOST_PP_CAT(P, n) >::type \
//!

#include <lslboost/container/detail/config_end.hpp>

//#else

//#ifdef BOOST_CONTAINER_PERFECT_FORWARDING
//#error "This file is not needed when perfect forwarding is available"
//#endif   //BOOST_CONTAINER_PERFECT_FORWARDING

#endif //#ifndef BOOST_CONTAINER_DETAIL_PREPROCESSOR_HPP
