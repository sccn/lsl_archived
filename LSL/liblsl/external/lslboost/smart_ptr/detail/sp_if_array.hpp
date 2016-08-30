/*
 * Copyright (c) 2012 Glen Joseph Fernandes 
 * glenfe at live dot com
 *
 * Distributed under the Boost Software License, 
 * Version 1.0. (See accompanying file LICENSE_1_0.txt 
 * or copy at http://lslboost.org/LICENSE_1_0.txt)
 */
#ifndef BOOST_SMART_PTR_DETAIL_SP_IF_ARRAY_HPP
#define BOOST_SMART_PTR_DETAIL_SP_IF_ARRAY_HPP

#include <lslboost/smart_ptr/shared_ptr.hpp>

namespace lslboost {
    namespace detail {
        template<typename T> 
        struct sp_if_array;
        template<typename T>
        struct sp_if_array<T[]> {
            typedef lslboost::shared_ptr<T[]> type;
        };
        template<typename T>
        struct sp_if_size_array;
        template<typename T, std::size_t N>
        struct sp_if_size_array<T[N]> {
            typedef lslboost::shared_ptr<T[N]> type;
        };
    }
}

#endif
