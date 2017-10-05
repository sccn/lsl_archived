cmake_minimum_required(VERSION 3.5)
project(LSLOutOfTreeTest)

# Most LSL Apps build in the same build tree, but external apps that link
# against a prebuilt liblsl are still supported. This tests if an app can
# - include the generated LSL::lsl target
# - compile and link successfully

# when building out of tree LSL_ROOT needs to be specified on the cmd line
if(NOT LSL_ROOT)
	message(FATAL_ERROR "When building out of tree you need to set LSL_ROOT ( cmake -DLSL_ROOT=/path/to/lsl/installation)")
endif()
message(STATUS "LSL install dir: ${LSL_ROOT}")
list(APPEND CMAKE_MODULE_PATH "${LSL_ROOT}/cmake")
include(LSLAppBoilerplate)

add_executable(LSLOutOfTreeTest OutOfTreeTest.c)
target_link_libraries(LSLOutOfTreeTest PRIVATE LSL::lsl)
add_custom_target(runOutOfTree
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LSL::lsl> $<TARGET_FILE_DIR:LSLOutOfTreeTest>
	COMMAND LSLOutOfTreeTest
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "run generated test executable"
)
