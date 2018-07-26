cmake_minimum_required (VERSION 3.5)
project (LSL
	VERSION 1.12.0
	LANGUAGES CXX)

cmake_policy(SET CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.
cmake_policy(SET CMP0063 NEW) # ENABLE CMP0063: Honor visibility for all targets
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if(NOT CMAKE_DEBUG_POSTFIX)
	set(CMAKE_DEBUG_POSTFIX -debug)
endif()
option(LSL_UNIXFOLDERS "Use the unix folder layout for install targets" Off)

# lsl uses boost, but several other projects (e.g. Matlab) also use boost.
# To prevent clashes with different boost versions, lsl ships a subset of
# boost with renamed symbols (lslboost::xy instead of boost::xy)
# If you need a specific boost version, you can put it in a subdirectory
# and add a CMakeLists.txt that exports the lslboost target
# (see lslboost/CMakeLists.txt for an example).
# If in doubt, use "external" (1.57 with patches) or "lslboost" (1.65.1)
set(LSL_LSLBOOST_PATH "external" CACHE STRING "Subdirectory with lslboost")

# Build static library
option (LSL_BUILD_STATIC "Also build static LSL library." OFF)

option(LSL_UNITTESTS "Build LSL library unit tests" OFF)

set (sources
	src/api_config.cpp
	src/api_config.h
	src/cancellable_streambuf.h
	src/cancellation.h
	src/common.cpp
	src/common.h
	src/consumer_queue.cpp
	src/consumer_queue.h
	src/data_receiver.cpp
	src/data_receiver.h
	src/endian/conversion.hpp
	src/endian/detail/intrinsic.hpp
	src/info_receiver.cpp
	src/info_receiver.h
	src/inlet_connection.cpp
	src/inlet_connection.h
	src/lsl_continuous_resolver_c.cpp
	src/lsl_freefuncs_c.cpp
	src/lsl_inlet_c.cpp
	src/lsl_outlet_c.cpp
	src/lsl_streaminfo_c.cpp
	src/lsl_xml_element_c.cpp
	src/portable_archive/portable_archive_exception.hpp
	src/portable_archive/portable_iarchive.hpp
	src/portable_archive/portable_oarchive.hpp
	src/pugixml/pugiconfig.hpp
	src/pugixml/pugixml.cpp
	src/pugixml/pugixml.hpp
	src/resolver_impl.cpp
	src/resolver_impl.h
	src/resolve_attempt_udp.cpp
	src/resolve_attempt_udp.h
	src/sample.cpp
	src/sample.h
	src/send_buffer.cpp
	src/send_buffer.h
	src/socket_utils.cpp
	src/socket_utils.h
	src/stream_info_impl.cpp
	src/stream_info_impl.h
	src/stream_inlet_impl.h
	src/stream_outlet_impl.cpp
	src/stream_outlet_impl.h
	src/tcp_server.cpp
	src/tcp_server.h
	src/time_postprocessor.cpp
	src/time_postprocessor.h
	src/time_receiver.cpp
	src/time_receiver.h
	src/udp_server.cpp
	src/udp_server.h
)
if(MSVC)
	list(APPEND sources src/legacy/legacy_abi.cpp src/legacy/legacy_abi.h)
endif()

set(headers
	include/lsl_c.h
	include/lsl_cpp.h
)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(lslplatform 64)
else()
	set(lslplatform 32)
endif()

# try to find out which revision is currently checked out
find_package(Git)
if(GIT_FOUND)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --tags HEAD
		WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
		OUTPUT_VARIABLE lslgitrevision
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
else()
	set(lslgitrevision "unknown")
endif()
set(LSL_VERSION_INFO "\"git:${lslgitrevision}/build:${CMAKE_BUILD_TYPE}/compiler:${CMAKE_CXX_COMPILER_ID}-${CMAKE_CXX_COMPILER_VERSION}/boost:\" BOOST_LIB_VERSION")
set_source_files_properties("src/lsl_freefuncs_c.cpp" PROPERTIES COMPILE_DEFINITIONS LSL_LIBRARY_INFO_STR=${LSL_VERSION_INFO})

set(target lsl)
set(lsl_export_targets ${target})

add_subdirectory(${LSL_LSLBOOST_PATH})

# common definitions for the lsl and lsl-static
function(lsllib_properties libname)
	target_link_libraries(${libname} PRIVATE lslboost)
	target_include_directories(${libname}
		INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
	)
	target_compile_definitions(${libname} PRIVATE
		LIBLSL_EXPORTS
		_SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS
		PUBLIC
		LSLNOAUTOLINK # don't use #pragma(lib) in CMake builds
	)
	if(NOT MSVC)
		target_compile_features(${libname} PRIVATE cxx_auto_type)
	endif()
endfunction()


# shared library
add_library(${target} SHARED ${sources} ${headers})
lsllib_properties(${target})
set_target_properties(${target} PROPERTIES
	VERSION ${LSL_VERSION_MAJOR}.${LSL_VERSION_MINOR}.${LSL_VERSION_PATCH}
	PREFIX ""
	OUTPUT_NAME "lib${target}${lslplatform}"
)

if(${LSL_BUILD_STATIC})
	add_library(${target}-static ${sources} ${headers})
	lsllib_properties(${target}-static)
	list(APPEND lsl_export_targets "${target}-static")
	# for LSL_CPP_API export header
	target_compile_definitions(${target}-static PRIVATE LIBLSL_STATIC)
	set_target_properties(${target}-static PROPERTIES
		OUTPUT_NAME "${target}${lslplatform}-static")
endif()

if (MINGW)
	target_link_libraries (${target} PUBLIC ws2_32 wsock32 winmm)
endif()

if(LSL_UNITTESTS)
	add_subdirectory(testing/UnitTests)
endif()

if(LSL_UNIXFOLDERS)
	set(LSLPREFIX "")
else()
	set(LSLPREFIX "LSL/")
endif()

install(TARGETS ${lsl_export_targets}
	COMPONENT liblsl
	EXPORT "${PROJECT_NAME}Config"
	RUNTIME DESTINATION ${LSLPREFIX}lib
	LIBRARY DESTINATION ${LSLPREFIX}lib
	INCLUDES DESTINATION ${LSLPREFIX}include
	ARCHIVE DESTINATION ${LSLPREFIX}lib
)

install(EXPORT "${PROJECT_NAME}Config"
	COMPONENT liblsl
	NAMESPACE "LSL::"
	DESTINATION "${LSLPREFIX}share/LSL")

# install headers
install(FILES ${headers}
	COMPONENT liblsl
	DESTINATION "${LSLPREFIX}include"
)

install(FILES LSLCMake.cmake
	COMPONENT liblsl
	DESTINATION "${LSLPREFIX}share/LSL")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
include(LSLCMake)
LSLGenerateCPackConfig()
