cmake_minimum_required (VERSION 3.5)
project (LSL
	VERSION 1.4.0
	LANGUAGES CXX)

cmake_policy(SET CMP0042 NEW) # ENABLE CMP0042: MACOSX_RPATH is enabled by default.
if(NOT CMAKE_DEBUG_POSTFIX)
	set(CMAKE_DEBUG_POSTFIX -debug)
endif()

# Allow to use either the operating system supplied boost distribution or the shrink-wrapped one in the external/ directory
option (LSL_USE_SYSTEM_BOOST "Use system Boost libraries instead of the version shipped with LSL." OFF)

# Build static library
option (LSL_BUILD_STATIC "Also build static LSL library." OFF)

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
	src/dllmain.cpp
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
if(WIN32)
	list(APPEND sources src/legacy/legacy_abi.cpp src/legacy/legacy_abi.h)
endif()

set(headers
	include/lsl_c.h
	include/lsl_cpp.h
)

set(target lsl)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(lslplatform 64)
else()
	set(lslplatform 32)
endif()
set(lslplatform ${lslplatform})

set(lsl_export_targets ${target})

if (LSL_USE_SYSTEM_BOOST)
	if(WIN32)
		set(Boost_USE_STATIC_LIBS On)
	endif()
	find_package(Boost COMPONENTS filesystem serialization thread REQUIRED)
	set(LSL_BOOST_LIBS Boost::filesystem Boost::serialization Boost::thread)
else ()
	add_subdirectory (external)
	set(LSL_BOOST_LIBS "lslboost")
endif ()

# common definitions for the lsllib variants
function(lsllib_properties libname)
	target_include_directories(${libname}
		PRIVATE
		${CMAKE_CURRENT_BINARY_DIR} # generated export header
		INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
	)
	message(STATUS "Linking LSL against: ${LSL_BOOST_LIBS}")
	message(STATUS "Found Boost libraries: ${Boost_LIBRARIES}")
	target_link_libraries(${libname} PRIVATE ${LSL_BOOST_LIBS})
	target_compile_definitions(${libname} PRIVATE LSL_INCLUDE_EXPORT_DEFINES)
endfunction()


# shared library
add_library(${target} SHARED ${sources} ${headers})
lsllib_properties(${target})
set_target_properties(${target} PROPERTIES
	PREFIX "lib"
	OUTPUT_NAME "${target}${lslplatform}"
)

if(${LSL_BUILD_STATIC})
	add_library(${target}-static ${sources} ${headers})
	lsllib_properties(${target}-static)
	list(APPEND lsl_export_targets "${target}-static")
	# for LSL_CPP_API export header
	target_compile_definitions(${target}-static PRIVATE LSL_STATIC_DEFINE)
	set_target_properties(${target}-static PROPERTIES
		OUTPUT_NAME "${target}${lslplatform}-static")
endif()

# Generate with LIBLSL_CPP_API exports
include(GenerateExportHeader)
generate_export_header(${target}
	BASE_NAME LSL
	EXPORT_MACRO_NAME LIBLSL_CPP_API
	EXPORT_FILE_NAME lsl_export_defines.h)

set_target_properties(${target} PROPERTIES
	VERSION ${LSL_VERSION_MAJOR}.${LSL_VERSION_MINOR}.${LSL_VERSION_PATCH}
	#SOVERSION ${LSL_VERSION_MAJOR}.0.0 # hopefully backwards compatible symlink
	CXX_VISIBILITY_PRESET hidden # only export symbols marked with LIBLSL_CPP_API
)

# TODO: Need to check if all flags are necessary or if some flags are missing
if (WIN32)
	if (MINGW)
		target_link_libraries (${target} winmm ws2_32 wsock32)
	endif()

	target_compile_definitions(${target} PUBLIC
		BOOST_ALL_NO_LIB # only link explicitly via target_link_libraries
		LIBLSL_EXPORTS
		_SCL_SECURE_NO_WARNINGS
		_CRT_SECURE_NO_WARNINGS
	)
endif ()

install(TARGETS ${lsl_export_targets}
	COMPONENT liblsl
  EXPORT "${PROJECT_NAME}Config"
  RUNTIME DESTINATION LSL/lib
  LIBRARY DESTINATION LSL/lib
  INCLUDES DESTINATION LSL/include
  ARCHIVE DESTINATION LSL/lib
)
# TODO: where to install this, see https://stackoverflow.com/a/10766438 
install(EXPORT "${PROJECT_NAME}Config"
	COMPONENT liblslheaders
   NAMESPACE "LSL::"
   DESTINATION LSL/cmake)

# install headers
install(FILES ${headers}
	COMPONENT "liblsl-dev"
  DESTINATION LSL/include/
)
set(CPACK_COMPONENT_liblsl-dev_DEPENDS liblsl GLOBAL_SCOPE)
