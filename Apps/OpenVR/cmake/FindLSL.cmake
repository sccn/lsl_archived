# - Try to find the labstreaminglayer library
#
#  LSL_FOUND - system has lsl
#  LSL_INCLUDE_DIRS - the lsl include directory
#  LSL_LIBRARIES - Link these to use lsl
#  LSL_BINARIES

set(LSL_ROOT_DIR
    "${LSL_ROOT_DIR}"
    CACHE
    PATH
    "Directory to search for LabStreamingLayer API")


IF (LSL_INCLUDE_DIRS AND LSL_LIBRARIES)

    # in cache already
    set(LSL_FOUND TRUE)

ELSE (LSL_INCLUDE_DIRS AND LSL_LIBRARIES)
    IF(NOT LSL_ROOT_DIR)
        message(STATUS "LSL_ROOT_DIR not set. Use `cmake [...] -DLSL_ROOT_DIR=<path/to/LSL/liblsl>")
        message("\tDefaulting to ${CMAKE_CURRENT_LIST_DIR}/../../../LSL/liblsl")
        SET(LSL_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../LSL/liblsl CACHE PATH "Path to local liblsl")
    ENDIF()
    
    FIND_PATH(LSL_INCLUDE_DIRS
        NAMES
            lsl_cpp.h
        PATHS 
            ${LSL_ROOT_DIR}/include
    )
    
    IF (${CMAKE_C_SIZEOF_DATA_PTR} EQUAL 8)
        SET(_arch 64)
    ELSE()
        SET(_arch 32)
    ENDIF()
    
    # LSL prefixes all libs with 'lib'.
    # This is expected in UNIX.
    SET(MY_PREFIX "")
    # But is unusual in Windows.
    IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        SET(MY_PREFIX lib)
    ENDIF()
    
    find_library(LSL_LIBRARY_RELEASE
        NAMES ${MY_PREFIX}lsl${_arch}
        PATHS ${LSL_ROOT_DIR}/bin
    )
    
    find_library(LSL_LIBRARY_DEBUG
        NAMES ${MY_PREFIX}lsl${_arch}-debug
        PATHS ${LSL_ROOT_DIR}/bin
    )
    SET(LSL_LIBRARIES
        debug ${LSL_LIBRARY_DEBUG}
        optimized ${LSL_LIBRARY_RELEASE})
    
    find_file(LSL_BINARY_RELEASE
        NAMES liblsl${_arch}${CMAKE_SHARED_LIBRARY_SUFFIX}
        PATHS ${LSL_ROOT_DIR}/bin
    )
    find_file(LSL_BINARY_DEBUG
        NAMES liblsl${_arch}-debug${CMAKE_SHARED_LIBRARY_SUFFIX}
        PATHS ${LSL_ROOT_DIR}/bin
    )

    INCLUDE(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(LSL DEFAULT_MSG LSL_INCLUDE_DIRS LSL_LIBRARIES LSL_BINARY_DEBUG LSL_BINARY_RELEASE)
    MARK_AS_ADVANCED(LSL_INCLUDE_DIRS LSL_LIBRARIES LSL_BINARY_DEBUG LSL_BINARY_RELEASE)

ENDIF (LSL_INCLUDE_DIRS AND LSL_LIBRARIES)