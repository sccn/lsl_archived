# CMake version
cmake_minimum_required(VERSION 3.5)

project(LabRecorder
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
	file(TO_CMAKE_PATH ${LSL_INSTALL_ROOT} LSL_INSTALL_ROOT)
	list(APPEND CMAKE_MODULE_PATH "${LSL_INSTALL_ROOT}/cmake")
    include(LSLAppBoilerplate)
endif()

# GENERAL CONFIG #
cmake_policy(SET CMP0028 NEW) # ENABLE CMP0028: Double colon in target name means ALIAS or IMPORTED target.
cmake_policy(SET CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
cmake_policy(SET CMP0063 NEW) # ENABLE CMP0063: Honor visibility properties for all target types.
cmake_policy(SET CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.

# Meta information about the project
set(META_PROJECT_DESCRIPTION "Record LabStreamingLayer streams to XDF data file.")

# THIRD PARTY LIBRARIES #
find_package(Qt5 REQUIRED COMPONENTS Network Xml Widgets)
#TODO: Make sure the architecture of the Qt binaries matches the current context architecture.

# Boost
IF(MSVC)
    # Default location of pre-compiled Boost for Windows
    # Override by adding "-DBOOST_ROOT=C:\path\to\boost\ -DBOOST_LIBRARYDIR=C:\path\to\boost\lib32-msvc-14.0\" to your cmake command
    IF (NOT BOOST_ROOT)
        SET(BOOST_ROOT "C:/local/boost_1_63_0/")
        SET(BOOST_LIBRARYDIR "C:/local/boost_1_63_0/lib64-msvc-14.0/")
        ENDIF()
ENDIF()

# APPLICATION #

# Target name
set(target LabRecorder)

# Build executable
add_executable(${target}
    MACOSX_BUNDLE
    WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    recording.h
    targetver.h
)

set(Boost_NO_SYSTEM_PATHS true)
set (Boost_USE_STATIC_LIBS OFF CACHE BOOL "use static libraries from Boost")
set (Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system chrono date_time filesystem iostreams thread zlib)
include_directories(${Boost_INCLUDE_DIRS})
#link_libraries(${Boost_LIBRARIES})

if (WIN32)
  # disable autolinking in boost
  add_definitions( -DBOOST_ALL_NO_LIB )

  # force all boost libraries to dynamic link (we already disabled
  # autolinking, so I don't know why we need this, but we do!)
  #add_definitions( -DBOOST_ALL_DYN_LINK )
endif()

#IF(MSVC)
#    find_package(Boost REQUIRED COMPONENTS filesystem iostreams thread zlib)
#    set(zlib_libs Boost::zlib)
#ELSE()
#    find_package(Boost REQUIRED COMPONENTS filesystem iostreams thread)
#    find_package(ZLIB REQUIRED)
#    set(zlib_libs ${ZLIB_LIBRARIES})
#ENDIF()
#INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${BOOST_ROOT})
target_link_libraries(${target}
    PRIVATE
    Qt5::Widgets
    Qt5::Network
	${Boost_LIBRARIES}
    #Boost::filesystem
    #Boost::iostreams
    #Boost::thread
    #${zlib_libs}
    LSL::lsl
)

# Fix a bug where building debug tries to link with non-debug lib.
# This should rather be addressed in the default linking pragmas
#if(MSVC)
#   SET_TARGET_PROPERTIES(${target} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:\"liblsl64\"")
#endif(MSVC)

installLSLApp(${target})
installLSLAuxFiles(${target} default_config.cfg)
