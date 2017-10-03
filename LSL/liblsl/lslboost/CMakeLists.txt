# Platform-independent sources
file(STRINGS lslboost_sources.txt lslboost_sources)
file(STRINGS lslboost_headers.txt lslboost_headers)
if (UNIX)
	file(STRINGS lslboost_sources_unix.txt lslboost_sources_platform)
	set(Boost_LIBRARIES pthread)
else ()  # WIN32
	file(STRINGS lslboost_sources_win.txt lslboost_sources_platform)
	set(Boost_LIBRARIES "")
endif ()

add_library (lslboost ${lslboost_sources} ${lslboost_sources_platform} ${lslboost_headers})
set_property (TARGET lslboost PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(lslboost PUBLIC BOOST_ALL_NO_LIB=1 BOOST_THREAD_BUILD_LIB=1)
target_include_directories(lslboost PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(lslboost PRIVATE ${Boost_LIBRARIES})
set_target_properties(lslboost PROPERTIES CXX_VISIBILITY_PRESET default)

# gcc 5 needs -std=c++0x to compile lslboost
if(NOT MSVC)
  target_compile_definitions(lslboost PRIVATE cxx_auto_type)
endif ()

install(TARGETS lslboost EXPORT lslboostConfig
	ARCHIVE DESTINATION LSL/lib
	LIBRARY DESTINATION LSL/lib)
#install(DIRECTORY boost lslboost
#	DESTINATION LSL/include)
#install(EXPORT lslboostConfig
#  DESTINATION LSL/cmake)
