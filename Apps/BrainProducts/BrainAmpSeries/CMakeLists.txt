cmake_minimum_required(VERSION 3.5)
project(BrainAmpSeries
	LANGUAGES CXX
	VERSION 0.1.0)

# load LSLAppBoilerplate if not done already
if(NOT TARGET LSL::lsl)
	# when building out of tree LSL_ROOT needs to be specified on the cmd line
	file(TO_CMAKE_PATH "${LSL_INSTALL_ROOT}" LSL_INSTALL_ROOT)
	list(APPEND LSL_INSTALL_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../LSL/liblsl/build/install")
	find_package(LSL HINTS ${LSL_INSTALL_ROOT}/share/LSL/ QUIET)
	if(NOT LSL_FOUND)
		message(FATAL_ERROR "Precompiled LSL was not found. Set LSL_INSTALL_ROOT to the LSL installation path ( cmake -DLSL_INSTALL_ROOT=/path/to/installed/lsl)")
	endif()
	list(APPEND CMAKE_MODULE_PATH ${LSL_DIR})
	message(STATUS "Looking for LSLCMake in ${LSL_DIR}")
	include(LSLCMake)
endif()

# GENERAL CONFIG #
set(META_PROJECT_DESCRIPTION "Stream from BrainAmp devices via Lab Streaming Layer")

# THIRD PARTY LIBRARIES #
find_package(Qt5 COMPONENTS Xml Widgets REQUIRED)

# Target name
set(target BrainAmpSeries)

# Sources
set(sources
	main.cpp
	mainwindow.cpp
	mainwindow.h
	mainwindow.ui
	BrainAmpIoCtl.h
)

# Add executable target
add_executable(${target} MACOSX_BUNDLE WIN32 ${sources})

target_link_libraries(${target}
	PRIVATE
	Qt5::Widgets
	Qt5::Xml
	LSL::lsl
)

if(UNIX)
  target_link_libraries(${target} PRIVATE pthread) # for std::thread
endif()

# Deployment
installLSLApp(${target})
installLSLAuxFiles(${target} BrainAmpSeries_config.cfg
	explanation_of_trigger_marker_types.pdf)
