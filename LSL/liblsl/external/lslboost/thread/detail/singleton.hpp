// Copyright (C) 2001-2003
// Mac Murrett
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org for most recent version including documentation.

#ifndef BOOST_SINGLETON_MJM012402_HPP
#define BOOST_SINGLETON_MJM012402_HPP

#include <lslboost/thread/detail/config.hpp>

namespace lslboost {
namespace detail {
namespace thread {

// class singleton has the same goal as all singletons: create one instance of
// a class on demand, then dish it out as requested.

template <class T>
class singleton : private T
{
private:
    singleton();
    ~singleton();

public:
    static T &instance();
};


template <class T>
inline singleton<T>::singleton()
{
    /* no-op */
}

template <class T>
inline singleton<T>::~singleton()
{
    /* no-op */
}

template <class T>
/*static*/ T &singleton<T>::instance()
{
    // function-local static to force this to work correctly at static
    // initialization time.
    static singleton<T> s_oT;
    return(s_oT);
}

} // namespace thread
} // namespace detail
} // namespace lslboost

#endif // BOOST_SINGLETON_MJM012402_HPP
