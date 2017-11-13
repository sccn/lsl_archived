cmake_minimum_required(VERSION 3.0)
project(LSLOpenVR VERSION 0.0.1)

# General Config #

# load LSLAppBoilerplate if not done already
if(NOT TARGET LSL::lsl)
	if(NOT LSL_INSTALL_ROOT)
		set(LSL_INSTALL_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../build/lsl_Release/lslinstall/LSL")
		get_filename_component(LSL_INSTALL_ROOT ${LSL_INSTALL_ROOT} ABSOLUTE)
		if(NOT EXISTS ${LSL_INSTALL_ROOT})
			message(FATAL_ERROR "When building a single app out of tree you need to set LSL_INSTALL_ROOT ( cmake -DLSL_INSTALL_ROOT=/path/to/installed/lsl)")
		endif()
	endif()
	if(NOT IS_ABSOLUTE ${LSL_INSTALL_ROOT})
		message(FATAL_ERROR "LSL_INSTALL_ROOT needs to be an absolute path")
	endif()
	file(TO_CMAKE_PATH ${LSL_INSTALL_ROOT} LSL_INSTALL_ROOT)
	list(APPEND CMAKE_MODULE_PATH "${LSL_INSTALL_ROOT}/cmake")
    include(LSLAppBoilerplate)
endif()

set(CMAKE_INSTALL_PREFIX ${LSL_INSTALL_ROOT}/..)

set(CMAKE_CXX_STANDARD 11)

# THIRD PARTY LIBRARIES #

# Qt
if(WIN32 AND NOT Qt5_DIR)
    set(Qt5_DIR "C:\\Qt\\5.9.1\\msvc2015_64\\lib\\cmake\\Qt5")
endif()
find_package(Qt5 REQUIRED COMPONENTS Core Network Xml Gui Widgets)

# OpenVR
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(ExternalProject)
IF(NOT OpenVR_ROOT_DIR)
    message(STATUS "OpenVR_ROOT_DIR not set.")
    message("\tUse cmake [...] -DOpenVR_ROOT_DIR=<path/to/openvr>")
    message("\tAttempting Chad's default rel directory...")
    SET(OpenVR_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../VR/openvr")
ENDIF()
find_package(OpenVR)
IF(NOT OpenVR_FOUND)
    message(STATUS "OpenVR not found. It will be downloaded from GitHub during configure. This can be very slow.")
    ExternalProject_Add(openvr
        PREFIX ${CMAKE_CURRENT_LIST_DIR}/deps/openvr
        DOWNLOAD_NO_PROGRESS 0  # Do not disable download progress.
        GIT_REPOSITORY https://github.com/ValveSoftware/openvr.git
        GIT_SHALLOW 1
        BUILD_COMMAND "" # Disable build step.
        INSTALL_COMMAND "" # Disable install step.
    )
    ExternalProject_Get_Property(openvr SOURCE_DIR)
    # Cannot use find_package because the project is not downloaded yet.
    # Instead, assume file locations.
    set(SIZEOF_VOIDP ${CMAKE_SIZEOF_VOID_P})
    if((NOT APPLE) AND (CMAKE_SIZEOF_VOID_P EQUAL 8))
      set(PROCESSOR_ARCH "64")
    else()
      set(PROCESSOR_ARCH "32")
    endif()
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
    SET(OpenVR_INCLUDE_DIRS ${SOURCE_DIR}/headers)
    SET(OpenVR_BINARIES ${SOURCE_DIR}/bin/${PLATFORM_NAME}${PROCESSOR_ARCH}/${CMAKE_SHARED_LIBRARY_PREFIX}openvr_api${CMAKE_SHARED_LIBRARY_SUFFIX})
    SET(OpenVR_LIBRARIES ${SOURCE_DIR}/lib/${PLATFORM_NAME}${PROCESSOR_ARCH}/${CMAKE_SHARED_LIBRARY_PREFIX}openvr_api${CMAKE_IMPORT_LIBRARY_SUFFIX})
ENDIF()

###############
# APPLICATION #
###############
add_subdirectory(src) 
