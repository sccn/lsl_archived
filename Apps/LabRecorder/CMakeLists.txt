# CMake version
cmake_minimum_required(VERSION 3.5)

project(LabRecorder
    LANGUAGES CXX
	VERSION 1.1.2)

# load LSLAppBoilerplate if not done already
if(NOT TARGET LSL::lsl)
	# when building out of tree LSL_ROOT needs to be specified on the cmd line
	file(TO_CMAKE_PATH "${LSL_INSTALL_ROOT}" LSL_INSTALL_ROOT)
	list(APPEND LSL_INSTALL_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/build/install")
	find_package(LSL HINTS ${LSL_INSTALL_ROOT}/share/LSL/)
	if(NOT LSL_FOUND)
		message(FATAL_ERROR "Precompiled LSL was not found. Set LSL_INSTALL_ROOT to the LSL installation path ( cmake -DLSL_INSTALL_ROOT=/path/to/installed/lsl)")
	endif()
	list(APPEND CMAKE_MODULE_PATH ${LSL_DIR})
	message(STATUS "Looking for LSLCMake in ${LSL_DIR}")
	include(LSLCMake)
endif()

# GENERAL CONFIG #
cmake_policy(SET CMP0028 NEW) # ENABLE CMP0028: Double colon in target name means ALIAS or IMPORTED target.
cmake_policy(SET CMP0054 NEW) # ENABLE CMP0054: Only interpret if() arguments as variables or keywords when unquoted.
cmake_policy(SET CMP0063 NEW) # ENABLE CMP0063: Honor visibility properties for all target types.
cmake_policy(SET CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.

# Meta information about the project
set(META_PROJECT_DESCRIPTION "Record LabStreamingLayer streams to XDF data file.")

# THIRD PARTY LIBRARIES #
find_package(Qt5 REQUIRED COMPONENTS Network Widgets)

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
	recording.cpp
)
target_compile_features(${target} PRIVATE cxx_auto_type)

find_package(Boost REQUIRED COMPONENTS filesystem iostreams thread)
set(zlib_libs "")
if(WIN32)
	find_package(Boost OPTIONAL_COMPONENTS zlib QUIET)
	if(TARGET Boost::zlib)
		set(zlib_libs "Boost::zlib")
		message(STATUS "LabRecorder: Using Boost.zlib for xdfz support")

	endif()
else()
	find_package(ZLIB QUIET)
	if(ZLIB_FOUND)
		message(STATUS "LabRecorder: Using zlib for xdfz support")
		set(zlib_libs ${ZLIB_LIBRARIES})
	endif()
endif()

target_link_libraries(${target}
    PRIVATE
    Qt5::Widgets
    Qt5::Network
    Boost::filesystem
    Boost::iostreams
    Boost::thread
    ${zlib_libs}
    LSL::lsl
)

# Enable xdfz support if Boost.zlib (Windows) or plain zlib (Unix) was found
if(${zlib_libs})
	target_compile_definitions(${target} PRIVATE XDFZ_SUPPORT=1)
endif()

installLSLApp(${target})
installLSLAuxFiles(${target} default_config.cfg)

LSLGenerateCPackConfig()
