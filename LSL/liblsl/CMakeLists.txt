cmake_minimum_required (VERSION 2.6)
project (liblsl)

# Version number
set (liblsl_VERSION_MAJOR 1)
set (liblsl_VERSION_MINOR 4)
set (liblsl_VERSION_PATCH 0)

cmake_policy(SET CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX -debug)
endif()
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY         ${CMAKE_CURRENT_LIST_DIR}/bin )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_CURRENT_LIST_DIR}/bin )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY         ${CMAKE_CURRENT_LIST_DIR}/bin )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_CURRENT_LIST_DIR}/bin )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_CURRENT_LIST_DIR}/bin )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_CURRENT_LIST_DIR}/bin )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin )

# Allow to use either the operating system supplied boost distribution or the shrink-wrapped one in the external/ directory
option (USE_SYSTEM_BOOST "Use system Boost libraries instead of the version shipped with LSL." OFF)

# Build shared and/or static library
option (BUILD_SHARED "Build shared library." ON)
option (BUILD_STATIC "Build static library." OFF)

if (USE_SYSTEM_BOOST)
  find_package(Boost COMPONENTS chrono filesystem serialization system thread REQUIRED)
else ()
  add_subdirectory (external)
endif (USE_SYSTEM_BOOST)

add_subdirectory (src)
