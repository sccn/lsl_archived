# CMake version
cmake_minimum_required(VERSION 3.5)

project(Wiimote
    LANGUAGES CXX
    VERSION 0.1.0)
    
# load LSLAppBoilerplate if not done already
if(NOT TARGET LSL::lsl)
	if(NOT LSL_INSTALL_ROOT)
		message(FATAL_ERROR "When building a single app out of tree you need to set LSL_INSTALL_ROOT ( cmake -DLSL_INSTALL_ROOT=/path/to/installed/lsl)")
    endif()
	if(NOT IS_ABSOLUTE ${LSL_INSTALL_ROOT})
		message(FATAL_ERROR "LSL_INSTALL_ROOT needs to be an absolute path")
	endif()
	list(APPEND CMAKE_MODULE_PATH "${LSL_INSTALL_ROOT}/cmake")
	file(TO_CMAKE_PATH ${LSL_INSTALL_ROOT} LSL_INSTALL_ROOT)
    include(LSLAppBoilerplate)
endif()

set(CMAKE_INSTALL_PREFIX ${LSL_INSTALL_ROOT}/..)

# GENERAL CONFIG #

#
cmake_policy(SET CMP0028 NEW) # ENABLE CMP0028: Double colon in target name means ALIAS or IMPORTED target.
cmake_policy(SET CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
cmake_policy(SET CMP0063 NEW) # ENABLE CMP0063: Honor visibility properties for all target types.
cmake_policy(SET CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.

# Set runtime path
set(CMAKE_SKIP_BUILD_RPATH            FALSE) # Add absolute path to all dependencies for BUILD
set(CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE) # Use CMAKE_INSTALL_RPATH for INSTALL
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE) # Do NOT add path to dependencies for INSTALL
    
# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Meta information about the project
set(META_PROJECT_DESCRIPTION "Send Wii data streams (Wiimote, balanceboard, etc) to LSL Outlet.")
set(META_AUTHOR_ORGANIZATION "Ottawa Hospital Research Institute")
set(META_AUTHOR_DOMAIN       "https://github.com/sccn/labstreaminglayer/tree/master/Apps/Wiimote")
set(META_AUTHOR_MAINTAINER   "Chadwick Boulay -- chadwick<dot>boulay<at>gmail<dot>com")
set(META_VERSION_MAJOR       "0")
set(META_VERSION_MINOR       "1")
set(META_VERSION_PATCH       "0")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION}")


# External Dependencies #

# Qt
find_package(Qt5 REQUIRED COMPONENTS Core Network Gui Widgets)

#Wiiuse
include(ExternalProject)
set(WIIUSE_INSTALL_DIR ${CMAKE_CURRENT_LIST_DIR}/deps/local)
ExternalProject_Add(wiiuse
    PREFIX ${CMAKE_CURRENT_LIST_DIR}/deps/wiiuse
    INSTALL_DIR ${WIIUSE_INSTALL_DIR}
    GIT_REPOSITORY https://github.com/rpavlik/wiiuse.git
    GIT_TAG updates
    GIT_SHALLOW 1
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_INCLUDE_PATH=${WIIUSE_INSTALL_DIR}/include
        -DCMAKE_LIBRARY_PATH=${WIIUSE_INSTALL_DIR}/lib
        -DBUILD_WIIUSE_SHARED_LIB:BOOL=OFF
        -DBUILD_EXAMPLE:BOOL=OFF
        -DBUILD_EXAMPLE_SDL:BOOL=OFF
)
IF(APPLE)
    set(WIIUSE_LIB ${WIIUSE_INSTALL_DIR}/lib/libwiiuse.a)
ELSEIF(WIN32)
    add_definitions(-DWIIUSE_STATIC)
    set(WIIUSE_LIB
        debug ${WIIUSE_INSTALL_DIR}/lib/wiiuse_debug.lib
        optimized ${WIIUSE_INSTALL_DIR}/lib/wiiuse.lib)
ENDIF()

# Platform libraries required by Wiiuse
set(PLATFORM_LIBS)
if(APPLE)
    find_library(IOBLUETOOTH_FRAMEWORK
        NAMES
        IOBluetooth)
    find_library(COREFOUNDATION_FRAMEWORK
        NAMES
        CoreFoundation)
    find_library(FOUNDATION_FRAMEWORK
        NAMES
        Foundation)
    list(APPEND PLATFORM_LIBS ${IOBLUETOOTH_FRAMEWORK} ${COREFOUNDATION_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
else(WIN32)
    find_package(WinHID REQUIRED)
    include_directories(${WINHID_INCLUDE_DIRS})
    list(APPEND PLATFORM_LIBS ws2_32 setupapi ${WINHID_LIBRARIES})
endif(APPLE)


# Executable name and options #

# Target name
set(target Wiimote)

# Sources
set(sources
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

# Build executable
add_executable(${target}
    MACOSX_BUNDLE
    ${sources}
)

add_dependencies(${target} wiiuse)

target_include_directories(${target}
    PRIVATE
    ${WIIUSE_INSTALL_DIR}/include)

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Network
    LSL::lsl
    ${WIIUSE_LIB}
    ${PLATFORM_LIBS}
)


# Deployment #
installLSLApp(${target})
