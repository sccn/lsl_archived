# - try to find the OpenVR SDK - currently designed for the version on GitHub.
#
# Cache Variables: (probably not for direct use in your scripts)
#  OPENVR_INCLUDE_DIR
#
# Non-cache variables you might use in your CMakeLists.txt:
#  OpenVR_FOUND
#  OpenVR_INCLUDE_DIRS
#  OpenVR_LIBRARIES
#  OpenVR_BINARIES
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)
#
# Original Author:
# 2015 Ryan A. Pavlik <ryan@sensics.com>
# Modified by:
# 2017 Chadwick Boulay <chadwick.boulay@gmail.com>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

set(OpenVR_ROOT_DIR
    "${OpenVR_ROOT_DIR}"
    CACHE
    PATH
    "Directory to search for OpenVR SDK")

# Check if 32 or 64 bit system.
set(SIZEOF_VOIDP ${CMAKE_SIZEOF_VOID_P})
if((NOT APPLE) AND (CMAKE_SIZEOF_VOID_P EQUAL 8))
    #64-bit not available in Mac.
    set(PROCESSOR_ARCH "64")
else()
    set(PROCESSOR_ARCH "32")
endif()

# Get platform.
if(WIN32)
    set(PLATFORM_NAME "win")
elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES ".*Linux")
        set(PLATFORM_NAME "linux")
    endif()
elseif(APPLE)
    if(CMAKE_SYSTEM_NAME MATCHES ".*Darwin.*" OR CMAKE_SYSTEM_NAME MATCHES ".*MacOS.*")
        set(PLATFORM_NAME "osx")
    endif()
endif()

find_path(OPENVR_INCLUDE_DIR
    NAMES
        openvr_driver.h
    PATHS
        ${OpenVR_ROOT_DIR}/headers
)
list(APPEND OpenVR_INCLUDE_DIRS ${OPENVR_INCLUDE_DIR})

find_library(OpenVR_LIBRARIES
    NAMES
        openvr_api
    PATHS
        ${OpenVR_ROOT_DIR}/lib/${PLATFORM_NAME}${PROCESSOR_ARCH}
)

find_file(OpenVR_BINARIES
    NAMES
        ${CMAKE_SHARED_LIBRARY_PREFIX}openvr_api${CMAKE_SHARED_LIBRARY_SUFFIX}
    PATHS
        ${OpenVR_ROOT_DIR}/bin/${PLATFORM_NAME}${PROCESSOR_ARCH}
)

SET(OpenVR_FOUND TRUE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenVR
    DEFAULT_MSG
    OpenVR_FOUND
    OpenVR_INCLUDE_DIRS
    OpenVR_LIBRARIES
    OpenVR_BINARIES)

mark_as_advanced(
    OpenVR_FOUND
    OpenVR_INCLUDE_DIRS
    OpenVR_LIBRARIES
    OpenVR_BINARIES)