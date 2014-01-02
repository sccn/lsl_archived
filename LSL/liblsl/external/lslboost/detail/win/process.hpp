//  process.hpp  --------------------------------------------------------------//

//  Copyright 2010 Vicente J. Botet Escriba

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.lslboost.org/LICENSE_1_0.txt


#ifndef BOOST_DETAIL_WIN_PROCESS_HPP
#define BOOST_DETAIL_WIN_PROCESS_HPP

#include <lslboost/detail/win/basic_types.hpp>
#include <lslboost/detail/win/GetCurrentProcess.hpp>

namespace lslboost {
namespace detail {
namespace win32 {
#if defined( BOOST_USE_WINDOWS_H )
    using ::GetCurrentProcessId;
#else
# ifndef UNDER_CE
extern "C" { 
    __declspec(dllimport) unsigned long __stdcall 
        GetCurrentProcessId(void);
}    
# else
    using ::GetCurrentProcessId;
# endif
#endif
}
}
}
#endif // BOOST_DETAIL_WIN_PROCESS_HPP
