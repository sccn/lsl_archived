project(Examples)

# If building the whole tree use the current LSL
if(TARGET lsl)
	add_library(LSL::lsl ALIAS lsl)
	message(STATUS "Target lsl found")
else()
	# otherwise find it somewhere else
	message(STATUS "Trying to find external liblsl")
	find_package(LSL REQUIRED)
endif()

# convenience function to add an example file
# this creates a target, links the necessary libraries and
# creates an install target
function(addlslexample name extension)
	add_executable(${name}
		${name}.${extension}
	)
	target_link_libraries(${name} LSL::lsl)
	target_compile_features(${name} PRIVATE cxx_constexpr)
	installLSLApp(${name})
endfunction()

addlslexample(CppSendRand cpp)
addlslexample(CppReceive cpp)
addlslexample(SendDataC c)
