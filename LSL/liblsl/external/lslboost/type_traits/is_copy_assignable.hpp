//  (C) Copyright Ion Gaztanaga 2014.
//
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.lslboost.org/LICENSE_1_0.txt).
//
//  See http://www.lslboost.org/libs/type_traits for most recent version including documentation.

#ifndef BOOST_TT_IS_COPY_ASSIGNABLE_HPP_INCLUDED
#define BOOST_TT_IS_COPY_ASSIGNABLE_HPP_INCLUDED

#include <lslboost/config.hpp>
#include <lslboost/type_traits/detail/yes_no_type.hpp>
#include <lslboost/type_traits/is_base_and_derived.hpp>
#include <lslboost/noncopyable.hpp>

#if !defined(BOOST_NO_CXX11_DELETED_FUNCTIONS) && !defined(BOOST_NO_CXX11_DECLTYPE) \
   && !defined(BOOST_INTEL_CXX_VERSION) && \
      !(defined(BOOST_MSVC) && _MSC_VER == 1800)
#define BOOST_TT_CXX11_IS_COPY_ASSIGNABLE
#include <lslboost/utility/declval.hpp>
#else
   //For compilers without decltype
   #include <lslboost/type_traits/is_const.hpp>
   #include <lslboost/type_traits/is_array.hpp>
   #include <lslboost/type_traits/add_reference.hpp>
   #include <lslboost/type_traits/remove_reference.hpp>
#endif


// should be the last #include
#include <lslboost/type_traits/detail/bool_trait_def.hpp>

namespace lslboost {

namespace detail{

template <bool DerivedFromNoncopyable, class T>
struct is_copy_assignable_impl2 {

// Intel compiler has problems with SFINAE for copy constructors and deleted functions:
//
// error: function *function_name* cannot be referenced -- it is a deleted function
// static lslboost::type_traits::yes_type test(T1&, decltype(T1(lslboost::declval<T1&>()))* = 0);
//                                                        ^ 
//
// MSVC 12.0 (Visual 2013) has problems when the copy constructor has been deleted. See:
// https://connect.microsoft.com/VisualStudio/feedback/details/800328/std-is-copy-constructible-is-broken
#if defined(BOOST_TT_CXX11_IS_COPY_ASSIGNABLE)
    typedef lslboost::type_traits::yes_type yes_type;
    typedef lslboost::type_traits::no_type  no_type;

    template <class U>
    static decltype(::lslboost::declval<U&>() = ::lslboost::declval<const U&>(), yes_type() ) test(int);

    template <class>
    static no_type test(...);

    static const bool value = sizeof(test<T>(0)) == sizeof(yes_type);

#else
    static BOOST_DEDUCED_TYPENAME lslboost::add_reference<T>::type produce();

    template <class T1>
    static lslboost::type_traits::no_type test(T1&, typename T1::lslboost_move_no_copy_constructor_or_assign* = 0);

    static lslboost::type_traits::yes_type test(...);
    // If you see errors like this:
    //
    //      `'T::operator=(const T&)' is private`
    //      `lslboost/type_traits/is_copy_assignable.hpp:NN:M: error: within this context`
    //
    // then you are trying to call that macro for a structure defined like that:
    //
    //      struct T {
    //          ...
    //      private:
    //          T & operator=(const T &);
    //          ...
    //      };
    //
    // To fix that you must modify your structure:
    //
    //      // C++03 and C++11 version
    //      struct T: private lslboost::noncopyable {
    //          ...
    //      private:
    //          T & operator=(const T &);
    //          ...
    //      };
    //
    //      // C++11 version
    //      struct T {
    //          ...
    //      private:
    //          T& operator=(const T &) = delete;
    //          ...
    //      };
    BOOST_STATIC_CONSTANT(bool, value = (
            sizeof(test(produce())) == sizeof(lslboost::type_traits::yes_type)
    ));
   #endif
};

template <class T>
struct is_copy_assignable_impl2<true, T> {
    BOOST_STATIC_CONSTANT(bool, value = false);
};

template <class T>
struct is_copy_assignable_impl {

#if !defined(BOOST_TT_CXX11_IS_COPY_ASSIGNABLE)
    //For compilers without decltype, at least return false on const types, arrays
    //types derived from lslboost::noncopyable and types defined as BOOST_MOVEABLE_BUT_NOT_COPYABLE
    typedef BOOST_DEDUCED_TYPENAME lslboost::remove_reference<T>::type unreferenced_t;
    BOOST_STATIC_CONSTANT(bool, value = (
        lslboost::detail::is_copy_assignable_impl2<
            lslboost::is_base_and_derived<lslboost::noncopyable, T>::value
            || lslboost::is_const<unreferenced_t>::value || lslboost::is_array<unreferenced_t>::value
            ,T
        >::value
    ));
    #else
    BOOST_STATIC_CONSTANT(bool, value = (
        lslboost::detail::is_copy_assignable_impl2<
            lslboost::is_base_and_derived<lslboost::noncopyable, T>::value,T
        >::value
    ));
    #endif
};

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_copy_assignable,T,::lslboost::detail::is_copy_assignable_impl<T>::value)
BOOST_TT_AUX_BOOL_TRAIT_SPEC1(is_copy_assignable,void,false)
#ifndef BOOST_NO_CV_VOID_SPECIALIZATIONS
BOOST_TT_AUX_BOOL_TRAIT_SPEC1(is_copy_assignable,void const,false)
BOOST_TT_AUX_BOOL_TRAIT_SPEC1(is_copy_assignable,void const volatile,false)
BOOST_TT_AUX_BOOL_TRAIT_SPEC1(is_copy_assignable,void volatile,false)
#endif

} // namespace lslboost

#include <lslboost/type_traits/detail/bool_trait_undef.hpp>

#endif // BOOST_TT_IS_COPY_ASSIGNABLE_HPP_INCLUDED
