//  (C) Copyright John Maddock 2005.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_TR1_FUNCTIONAL_HPP_INCLUDED
#  define BOOST_TR1_FUNCTIONAL_HPP_INCLUDED
#  include <lslboost/tr1/detail/config.hpp>
#  include <functional>

#if defined(BOOST_HAS_TR1_REFERENCE_WRAPPER) \
   || defined(BOOST_HAS_TR1_RESULT_OF)\
   || defined(BOOST_HAS_TR1_MEM_FN)\
   || defined(BOOST_HAS_TR1_BIND)\
   || defined(BOOST_HAS_TR1_FUNCTION)\
   || defined(BOOST_HAS_TR1_HASH)
#  if defined(BOOST_HAS_INCLUDE_NEXT) && !defined(BOOST_TR1_DISABLE_INCLUDE_NEXT)
#     include_next BOOST_TR1_HEADER(functional)
#  else
#     include <lslboost/tr1/detail/config_all.hpp>
#     include BOOST_TR1_STD_HEADER(BOOST_TR1_PATH(functional))
#  endif
#endif

#ifndef BOOST_HAS_TR1_REFERENCE_WRAPPER

#include <lslboost/ref.hpp>

namespace std{ namespace tr1{

   using ::lslboost::reference_wrapper;
   using ::lslboost::ref;
   using ::lslboost::cref;

} }

#endif  // BOOST_HAS_TR1_REFERENCE_WRAPPER

#if !defined(BOOST_HAS_TR1_RESULT_OF)\
   && !defined(BOOST_NO_SFINAE) && \
   !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

//
// we can only actually include result_of.hpp if the compiler
// really does support it, otherwise we just get endless errors...
//
#include <lslboost/utility/result_of.hpp>

namespace std{ namespace tr1{

   template<class F>
   struct result_of
     : ::lslboost::tr1_result_of<F>
   {};

} }

#endif // BOOST_HAS_TR1_RESULT_OF

#ifndef BOOST_HAS_TR1_MEM_FN
// mem_fn:
#include <lslboost/mem_fn.hpp>

namespace std{ namespace tr1{

using lslboost::mem_fn;

} }

#endif // BOOST_HAS_TR1_MEM_FN


#ifndef BOOST_HAS_TR1_BIND
// Bind:
#include <lslboost/bind.hpp>

namespace std{ namespace tr1{

   using ::lslboost::is_bind_expression;
   using ::lslboost::is_placeholder;
   using ::lslboost::bind;
   namespace placeholders {
#ifndef BOOST_BIND_NO_PLACEHOLDERS
      using ::_1;
      using ::_2;
      using ::_3;
      using ::_4;
      using ::_5;
      using ::_6;
      using ::_7;
      using ::_8;
      using ::_9;
#endif
   } // placeholders

} }

#endif

#ifndef BOOST_HAS_TR1_FUNCTION
// polymorphic function object wrappers:
#include <lslboost/function.hpp>
#include <lslboost/detail/workaround.hpp>

#if !BOOST_WORKAROUND(__BORLANDC__, < 0x582) \
    && !BOOST_WORKAROUND(BOOST_MSVC, < 1310) \
    && !defined(BOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX)
namespace std{ namespace tr1{

   using ::lslboost::bad_function_call;
   using ::lslboost::function;
   using ::lslboost::swap;

}}
#endif

#endif // BOOST_HAS_TR1_FUNCTION

#ifndef BOOST_HAS_TR1_HASH
//
// This header can get included by lslboost/hash.hpp
// leading to cyclic dependencies.  As a workaround
// we forward declare lslboost::hash and include
// the actual header later.
//
namespace lslboost{
template <class T> struct hash;
}

namespace std{ namespace tr1{
   //using ::lslboost::hash;

   template <class T>
   struct hash : public lslboost::hash<T>
   {
   };

}}

#include <lslboost/functional/hash.hpp>

#endif

#endif

