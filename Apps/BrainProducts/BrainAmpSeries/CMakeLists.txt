cmake_minimum_required(VERSION 3.5)
project(BrainAmpSeries
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
	include(LSLAppBoilerplate)
endif()

# GENERAL CONFIG #
set(META_PROJECT_DESCRIPTION "Stream from BrainAmp devices via Lab Streaming Layer")

# THIRD PARTY LIBRARIES #
find_package(Qt5 COMPONENTS Xml Widgets REQUIRED)
find_package(Boost REQUIRED)

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
# Boost includes needed here because they aren't pulled in by any linked library above
target_include_directories(${target} PRIVATE ${Boost_INCLUDE_DIRS})

if(UNIX)
  target_link_libraries(${target} PRIVATE pthread) # for std::thread
endif()

# Deployment
installLSLApp(${target})
install(FILES BrainAmpSeries_config.cfg
	DESTINATION ${PROJECT_NAME})
