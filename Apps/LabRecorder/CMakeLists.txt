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
		message(FATAL_ERROR "Precompiled LSL was not found. See https://github.com/labstreaminglayer/labstreaminglayer/blob/master/doc/BUILD.md#lsl_install_root for more information.")
	endif()
	list(APPEND CMAKE_MODULE_PATH ${LSL_DIR})
	message(STATUS "Looking for LSLCMake in ${LSL_DIR}")
	include(LSLCMake)
endif()

option(LABRECORDER_XDFZ "use Boost.Iostreams for XDFZ support" Off)
option(LABRECORDER_BOOST_TYPE_CONVERSIONS "Use boost for type conversions" Off)

# GENERAL CONFIG #
set(META_PROJECT_DESCRIPTION "Record LabStreamingLayer streams to XDF data file.")

# THIRD PARTY LIBRARIES #
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# APPLICATION #

# Target name
set(target ${PROJECT_NAME})

# Build executable
add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32
	main.cpp
	mainwindow.cpp
	mainwindow.h
	mainwindow.ui
	recording.h
	recording.cpp
	conversions.h
	xdfwriter.h
	xdfwriter.cpp
)

add_executable(testxdfwriter
	test_xdf_writer.cpp
	xdfwriter.h
	xdfwriter.cpp
)

target_compile_features(${target} PRIVATE cxx_auto_type cxx_lambda_init_captures)

find_package(Threads REQUIRED)

target_link_libraries(${target}
    PRIVATE
    Qt5::Widgets
    Threads::Threads
    LSL::lsl
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# Test for floating point format and endianness
try_run(IS_LITTLE_ENDIAN IS_IEC559
	${CMAKE_CURRENT_BINARY_DIR}
	"${CMAKE_CURRENT_SOURCE_DIR}/test_iec559_and_little_endian.cpp"
	CMAKE_FLAGS "-DCMAKE_CXX_STANDARD=14" "-DCMAKE_CXX_STANDARD_REQUIRED=On"
	COMPILE_OUTPUT_VARIABLE IEC559_COMPILE)
message(STATUS "Little endian: ${IS_LITTLE_ENDIAN}")
message(STATUS "IEC559: ${IS_IEC559}")
if(NOT IS_IEC559)
	message(WARNING "IEC559 test: ${IEC559_COMPILE}")
endif()


if(LABRECORDER_BOOST_TYPE_CONVERSIONS OR NOT IS_LITTLE_ENDIAN OR NOT IS_IEC559)
	message(STATUS "Trying to use Boost for type conversions")
	find_package(Boost REQUIRED)
	target_link_libraries(${target} PRIVATE Boost::boost)
	target_compile_definitions(${target} PRIVATE EXOTIC_ARCH_SUPPORT)
endif()

# Enable xdfz support if Boost::iostreams and Boost.zlib (Windows) or plain zlib (Unix) was found
if(LABRECORDER_XDFZ)
	find_package(Boost REQUIRED COMPONENTS iostreams)
	if(WIN32)
		find_package(Boost REQUIRED COMPONENTS zlib)
		target_link_libraries(${target} PRIVATE Boost::iostreams Boost::zlib)
	else()
		find_package(ZLIB REQUIRED)
		target_link_libraries(${target} PRIVATE Boost::iostreams ${ZLIB_LIBRARIES})
	endif()
	message(STATUS "Found zlib, enabling support for xdfz files")
	target_compile_definitions(${target} PRIVATE XDFZ_SUPPORT=1)
endif()

installLSLApp(${target})
installLSLAuxFiles(${target} default_config.cfg)

LSLGenerateCPackConfig()
