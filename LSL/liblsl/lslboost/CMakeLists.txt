# Platform-independent sources
file(STRINGS lslboost_sources.txt lslboost_sources)
file(STRINGS lslboost_headers.txt lslboost_headers)
if (UNIX)
	set(lslboost_sources_platform
		libs/thread/src/pthread/once.cpp
		libs/thread/src/pthread/once_atomic.cpp
		libs/thread/src/pthread/thread.cpp
	)
	set(Boost_LIBRARIES pthread)
else ()  # WIN32
	set(lslboost_sources_platform
		libs/filesystem/src/windows_file_codecvt.cpp
		libs/filesystem/src/path_traits.cpp
		libs/filesystem/src/codecvt_error_category.cpp
		libs/serialization/src/codecvt_null.cpp
		libs/thread/src/win32/thread.cpp
		libs/thread/src/win32/tss_dll.cpp
		libs/thread/src/win32/tss_pe.cpp
	)
	set(Boost_LIBRARIES "")
endif ()

add_library (lslboost STATIC ${lslboost_sources} ${lslboost_sources_platform} ${lslboost_headers})
target_compile_definitions(lslboost PUBLIC BOOST_ALL_NO_LIB=1 BOOST_THREAD_BUILD_LIB=1)
target_include_directories(lslboost PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(lslboost PRIVATE ${Boost_LIBRARIES})
set_target_properties(lslboost PROPERTIES
	CXX_VISIBILITY_PRESET hidden
	POSITION_INDEPENDENT_CODE On
)

# gcc 5 needs -std=c++0x to compile lslboost
if(NOT MSVC)
  target_compile_features(lslboost PRIVATE cxx_auto_type)
endif ()

install(TARGETS lslboost EXPORT lslboostConfig
	ARCHIVE DESTINATION LSL/lib)
install(EXPORT lslboostConfig
	DESTINATION LSL/cmake)
