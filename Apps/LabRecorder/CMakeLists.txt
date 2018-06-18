cmake_minimum_required(VERSION 3.5)

project(LabRecorder
	LANGUAGES CXX
	VERSION 1.12.0)

# set up LSL if not done already
if(NOT TARGET LSL::lsl)
	# when building out of tree LSL_ROOT needs to be specified on the cmd line
	file(TO_CMAKE_PATH "${LSL_INSTALL_ROOT}" LSL_INSTALL_ROOT)
	list(APPEND LSL_INSTALL_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/build/install")
	find_package(LSL HINTS ${LSL_INSTALL_ROOT}/share/LSL/ ${LSL_INSTALL_ROOT}/LSL/share/LSL QUIET)
	if(NOT LSL_FOUND)
		message(FATAL_ERROR "Precompiled LSL was not found. Set LSL_INSTALL_ROOT to the LSL installation path ( cmake -DLSL_INSTALL_ROOT=/path/to/installed/lsl)")
	endif()
	list(APPEND CMAKE_MODULE_PATH ${LSL_DIR})
	message(STATUS "Looking for LSLCMake in ${LSL_DIR}")
	include(LSLCMake)
endif()

# GENERAL CONFIG #
set(META_PROJECT_DESCRIPTION "Record LabStreamingLayer streams to XDF data file.")

# THIRD PARTY LIBRARIES #
find_package(Qt5 REQUIRED COMPONENTS Network Widgets)

# APPLICATION #

# Target name
set(target ${PROJECT_NAME})

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
target_compile_features(${target} PRIVATE cxx_auto_type cxx_lambda_init_captures)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED OPTIONAL_COMPONENTS iostreams)
if(WIN32)
	find_package(Boost OPTIONAL_COMPONENTS zlib QUIET)
	if(TARGET Boost::zlib)
		set(ZLIB_LIBRARIES Boost::zlib)
	endif()
else()
	find_package(ZLIB QUIET)
endif()

target_link_libraries(${target}
    PRIVATE
    Qt5::Widgets
    Qt5::Network
	Boost::boost
    Threads::Threads
    LSL::lsl
)

# Enable xdfz support if Boost::iostreams and Boost.zlib (Windows) or plain zlib (Unix) was found
if(ZLIB_LIBRARIES AND TARGET Boost::iostreams)
	message(STATUS "Found zlib, enabling support for xdfz files")
	target_link_libraries(${target}
		PRIVATE
		${ZLIB_LIBRARIES}
		Boost::iostreams
		)
	target_compile_definitions(${target} PRIVATE XDFZ_SUPPORT=1)
endif()

installLSLApp(${target})
installLSLAuxFiles(${target} default_config.cfg)

LSLGenerateCPackConfig()
