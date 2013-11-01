// Copyright (C) 2004 Peder Holt
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.lslboost.org/LICENSE_1_0.txt)

#ifndef BOOST_TYPE_TRAITS_MSVC_REMOVE_EXTENT_HOLT_2004_0827
#define BOOST_TYPE_TRAITS_MSVC_REMOVE_EXTENT_HOLT_2004_0827

#include <lslboost/type_traits/msvc/typeof.hpp>
#include <lslboost/type_traits/is_array.hpp>

namespace lslboost {
    namespace detail {
        template<bool IsArray>
        struct remove_extent_impl_typeof {
            template<typename T,typename ID>
            struct inner {
                typedef T type;
            };
        };
        template<>
        struct remove_extent_impl_typeof<true> {
            template<typename T,typename ID>
            struct inner {
                template<typename U>
                static msvc_register_type<U,ID> test(U[]);
                static msvc_register_type<T,ID> test(...);
                BOOST_STATIC_CONSTANT(unsigned,register_test=sizeof(test( *((T*)NULL) ) ));
                typedef typename msvc_extract_type<ID>::id2type::type type;
            };
        };
    } //namespace detail

    template<typename T>
    struct remove_extent {
        typedef typename lslboost::detail::remove_extent_impl_typeof<
            lslboost::is_array<T>::value                
        >::template inner<T,remove_extent<T> >::type type;
        BOOST_MPL_AUX_LAMBDA_SUPPORT(1,remove_extent,T)
    };
} //namespace lslboost

#endif //BOOST_TYPE_TRAITS_MSVC_REMOVE_BOUNDS_HOLT_2004_0827

