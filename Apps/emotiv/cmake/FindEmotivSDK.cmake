# - try to find the Emotiv SDK
#
#
# Non-cache variables you might use in your CMakeLists.txt:
#  EmotivSDK_FOUND
#  EmotivSDK_INCLUDE_DIRS
#  EmotivSDK_LIBRARIES
#  EmotivSDK_BINARIES
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)
#
# Authored by:
# 2017 Chadwick Boulay <chadwick.boulay@gmail.com>

IF(NOT EDK_ROOT)
    if(DEFINED ENV{EDK_ROOT})
        set(EDK_ROOT $ENV{EDK_ROOT})
    endif()
ENDIF()

IF(NOT EDK_ROOT)
    FILE(GLOB PATH_Candidates 
        "C:/Program Files (x86)/Emotiv SDK Premium Edition v3.3.3/EDK*" 
        "C:/Program Files (x86)/Emotiv SDK Premium Edition v3.3.2/EDK*"
    )
    FIND_PATH(EmotivSDK_INCLUDE_DIR Iedk.h
        PATH_SUFFIXES "Header files"
        PATHS ${PATH_Candidates}
    )
    IF(EmotivSDK_INCLUDE_DIR)
        set(EDK_ROOT ${EmotivSDK_INCLUDE_DIR}/../)
        get_filename_component(EDK_ROOT ${EDK_ROOT} ABSOLUTE)
    ENDIF()
ENDIF()

IF(NOT EDK_ROOT)
    message(status "EmotivSDK not found. Try 'set EDK_ROOT=<path/to/Emotiv/EDK>' then run cmake again.")
ENDIF()

# Check if 32 or 64 bit system.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PROCESSOR_ARCH "x64")
else()
    set(PROCESSOR_ARCH "x86")
endif()

find_path(EmotivSDK_INCLUDE_DIR
    NAMES
        Iedk.h
    PATHS
        "${EDK_ROOT}/Header files"
)
list(APPEND EmotivSDK_INCLUDE_DIRS ${EmotivSDK_INCLUDE_DIR})

find_library(EmotivSDK_LIBRARIES
    NAMES
        edk
    PATHS
        ${EDK_ROOT}/${PROCESSOR_ARCH}
)

find_file(EmotivSDK_BINARIES
    NAMES
        edk${CMAKE_SHARED_LIBRARY_SUFFIX}
    PATHS
        ${EDK_ROOT}/${PROCESSOR_ARCH}
)

SET(EmotivSDK_FOUND TRUE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EmotivSDK
    DEFAULT_MSG
    EmotivSDK_FOUND
    EmotivSDK_INCLUDE_DIRS
    EmotivSDK_LIBRARIES)

mark_as_advanced(
    EmotivSDK_FOUND
    EmotivSDK_INCLUDE_DIRS
    EmotivSDK_LIBRARIES
    EmotivSDK_BINARIES)