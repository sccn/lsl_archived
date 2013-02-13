cmake_minimum_required (VERSION 2.6)
project (liblsl)

# Version number
set (liblsl_VERSION_MAJOR 1)
set (liblsl_VERSION_MINOR 3)
set (liblsl_VERSION_PATCH 2)

option (USE_SYSTEM_BOOST "Use system Boost libraries instead of the version shipped with LSL." OFF)

if (USE_SYSTEM_BOOST)
  find_package(Boost COMPONENTS chrono filesystem serialization system thread REQUIRED)
else ()
  add_subdirectory (external)
endif (USE_SYSTEM_BOOST)

add_subdirectory (src)
