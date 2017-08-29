# - try to find the g.Tec g.NEEDaccess SDK
#
#
# Non-cache variables you might use in your CMakeLists.txt:
#  gNEEDaccessSDK_FOUND
#  gNEEDaccessSDK_INCLUDE_DIRS
#  gNEEDaccessSDK_LIBRARIES
#  gNEEDaccessSDK_BINARIES
#
# Requires these CMake modules:
#  FindPackageHandleStandardArgs (known included with CMake >=2.6.2)
#
# Authored by:
# 2017 Chadwick Boulay <chadwick.boulay@gmail.com>

message(STATUS "Trying to find package gNEEDaccessSDK")

IF(NOT GDS_ROOT)
    if(DEFINED ENV{GDS_ROOT})
        set(GDS_ROOT $ENV{GDS_ROOT})
    endif()
ENDIF()

IF(NOT GDS_ROOT)
    FILE(GLOB PATH_Candidates 
        "C:/Program Files/gtec/gNEEDaccess Client API/C*"
    )
    FIND_PATH(gNEEDaccessSDK_INCLUDE_DIR
        GDSClientAPI.h
        PATHS ${PATH_Candidates}
    )
    IF(gNEEDaccessSDK_INCLUDE_DIR)
        set(GDS_ROOT ${gNEEDaccessSDK_INCLUDE_DIR})
        get_filename_component(GDS_ROOT ${GDS_ROOT} ABSOLUTE)
    ENDIF()
ENDIF()

IF(NOT GDS_ROOT)
    message(status "gNEEDaccessSDK not found. Try 'set GDS_ROOT=<path/to/gtec/API/C>' then run cmake again.")
ENDIF()

# Check if 32 or 64 bit system.
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PROCESSOR_ARCH "x64")
else()
    set(PROCESSOR_ARCH "win32")
endif()

find_path(gNEEDaccessSDK_INCLUDE_DIR
    NAMES
        GDSClientAPI.h
    PATHS
        "${GDS_ROOT}"
)
list(APPEND gNEEDaccessSDK_INCLUDE_DIRS ${gNEEDaccessSDK_INCLUDE_DIR})

find_library(gNEEDaccessSDK_LIBRARIES
    NAMES
        GDSClientAPI  # Communicate with the server over the network (or local loopback). App may be on the same network or same computer as server.
        # GDSServer  # Apply commands directly onto the server without network communication. App must be on same computer as server.
    PATHS
        ${GDS_ROOT}/${PROCESSOR_ARCH}
)

find_file(gNEEDaccessSDK_BINARIES
    NAMES
        GDSClientAPI${CMAKE_SHARED_LIBRARY_SUFFIX}
        gAPI${CMAKE_SHARED_LIBRARY_SUFFIX}
        Networking${CMAKE_SHARED_LIBRARY_SUFFIX}
    PATHS
        ${GDS_ROOT}/${PROCESSOR_ARCH}
)

SET(gNEEDaccessSDK_FOUND TRUE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(gNEEDaccessSDK
    DEFAULT_MSG
    gNEEDaccessSDK_FOUND
    gNEEDaccessSDK_INCLUDE_DIRS
    gNEEDaccessSDK_LIBRARIES)

mark_as_advanced(
    gNEEDaccessSDK_FOUND
    gNEEDaccessSDK_INCLUDE_DIRS
    gNEEDaccessSDK_LIBRARIES
    gNEEDaccessSDK_BINARIES)