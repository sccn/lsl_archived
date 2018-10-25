cmake_minimum_required(VERSION 3.5)
project(LSLOutOfTreeTest
	LANGUAGES C)

# Most LSL Apps build in the same build tree, but external apps that link
# against a prebuilt liblsl are still supported. This tests if an app can
# - include the generated LSL::lsl target
# - compile and link successfully

# when building out of tree LSL_ROOT needs to be specified on the cmd line
file(TO_CMAKE_PATH "${LSL_INSTALL_ROOT}" LSL_INSTALL_ROOT)
list(APPEND LSL_INSTALL_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/build/install")
find_package(LSL HINTS ${LSL_INSTALL_ROOT}/share/LSL/ QUIET)
if(NOT LSL_FOUND)
	message(FATAL_ERROR "Precompiled LSL was not found. Set LSL_INSTALL_ROOT to the LSL installation path ( cmake -DLSL_INSTALL_ROOT=/path/to/installed/lsl)")
endif()
list(APPEND CMAKE_MODULE_PATH ${LSL_DIR})
include(LSLCMake)

add_executable(LSLOutOfTreeTest OutOfTreeTest.c)
target_link_libraries(LSLOutOfTreeTest PRIVATE LSL::lsl)
#add_executable(LSLOutOfTreeTest-static OutOfTreeTest.c)
#target_link_libraries(LSLOutOfTreeTest-static PRIVATE LSL::lsl-static)
add_custom_target(runOutOfTree
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LSL::lsl> $<TARGET_FILE_DIR:LSLOutOfTreeTest>
	COMMAND LSLOutOfTreeTest
	#	COMMAND LSLOutOfTreeTest-static
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "run generated test executable"
)
install(TARGETS LSLOutOfTreeTest
	RUNTIME DESTINATION "bin"
)
