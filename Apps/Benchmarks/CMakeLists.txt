cmake_minimum_required(VERSION 3.6)
project(Benchmarks)

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

function(addlslbench name extension)
	add_executable(${name}
		${name}.${extension}
	)
	target_link_libraries(${name} PRIVATE LSL::lsl)
	if(UNIX)
		target_link_libraries(${name} PRIVATE pthread)
	endif()
	target_compile_features(${name} PRIVATE cxx_constexpr)
	installLSLApp(${name})
endfunction()

addlslbench(Bounce cpp)
addlslbench(SpeedTest cpp)
#addlslbench(StressTest cpp)
#addlslbench(SyncTest cpp)
