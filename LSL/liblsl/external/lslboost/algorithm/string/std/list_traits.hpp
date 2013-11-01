//  Boost string_algo library list_traits.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org/ for updates, documentation, and revision history.

#ifndef BOOST_STRING_STD_LIST_TRAITS_HPP
#define BOOST_STRING_STD_LIST_TRAITS_HPP

#include <lslboost/algorithm/string/yes_no_type.hpp>
#include <list>
#include <lslboost/algorithm/string/sequence_traits.hpp>

namespace lslboost {
    namespace algorithm {

//  std::list<> traits  -----------------------------------------------//

#ifdef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

        // stable iterators tester
        template<typename T, typename AllocT>
        yes_type has_stable_iterators_tester( const ::std::list<T,AllocT>* );

        // const time insert tester
        template<typename T, typename AllocT>
        yes_type has_const_time_insert_tester( const ::std::list<T,AllocT>* );

        // const time erase tester
        template<typename T, typename AllocT>
        yes_type has_const_time_erase_tester( const ::std::list<T,AllocT>* );


#else // BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

        // stable iterators trait
        template<typename T, typename AllocT>
        class has_stable_iterators< ::std::list<T,AllocT> >
        {
        public:
#if BOOST_WORKAROUND( __IBMCPP__, <= 600 )
            enum { value = true };
#else
            BOOST_STATIC_CONSTANT(bool, value=true);
#endif // BOOST_WORKAROUND( __IBMCPP__, <= 600 )
            typedef mpl::bool_<has_stable_iterators<T>::value> type;
        };

        // const time insert trait
        template<typename T, typename AllocT>
        class has_const_time_insert< ::std::list<T,AllocT> >
        {
        public:
#if BOOST_WORKAROUND( __IBMCPP__, <= 600 )
            enum { value = true };
#else
            BOOST_STATIC_CONSTANT(bool, value=true);
#endif // BOOST_WORKAROUND( __IBMCPP__, <= 600 )
            typedef mpl::bool_<has_const_time_insert<T>::value> type;
        };

        // const time erase trait
        template<typename T, typename AllocT>
        class has_const_time_erase< ::std::list<T,AllocT> >
        {
        public:
#if BOOST_WORKAROUND( __IBMCPP__, <= 600 )
            enum { value = true };
#else
            BOOST_STATIC_CONSTANT(bool, value=true);
#endif // BOOST_WORKAROUND( __IBMCPP__, <= 600 )
            typedef mpl::bool_<has_const_time_erase<T>::value> type;
        };
#endif


    } // namespace algorithm
} // namespace lslboost


#endif  // BOOST_STRING_STD_LIST_TRAITS_HPP
