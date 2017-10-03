project(Benchmarks)

# If building the whole tree use the current LSL
if(TARGET lsl)
	add_library(LSL::lsl ALIAS lsl)
	message(STATUS "Target lsl found")
else()
	# otherwise find it somewhere else
	message(STATUS "Trying to find external liblsl")
	find_package(LSL REQUIRED)
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
