# Platform-independent sources
set(lslboost_sources
	libs/atomic/src/lockpool.cpp
	libs/chrono/src/chrono.cpp
	libs/program_options/src/convert.cpp
	libs/serialization/src/archive_exception.cpp
	libs/serialization/src/basic_archive.cpp
	libs/serialization/src/basic_iarchive.cpp
	libs/serialization/src/basic_iserializer.cpp
	libs/serialization/src/basic_oarchive.cpp
	libs/serialization/src/basic_oserializer.cpp
	libs/serialization/src/basic_serializer_map.cpp
	libs/serialization/src/extended_type_info.cpp
	libs/serialization/src/extended_type_info_typeid.cpp
	libs/serialization/src/singleton.cpp
	libs/serialization/src/void_cast.cpp
	libs/system/src/error_code.cpp
)
file(STRINGS lslboost_headers.txt lslboost_headers)
if (UNIX)
	set(lslboost_sources_platform
		libs/thread/src/pthread/once.cpp
		libs/thread/src/pthread/thread.cpp
	)
	set(Boost_LIBRARIES pthread)
else ()  # WIN32
	set(lslboost_sources_platform
		libs/serialization/src/codecvt_null.cpp
		libs/thread/src/win32/thread.cpp
		libs/thread/src/win32/tss_dll.cpp
		libs/thread/src/win32/tss_pe.cpp
	)
	set(Boost_LIBRARIES "")
endif ()

add_library (lslboost STATIC ${lslboost_sources} ${lslboost_sources_platform} ${lslboost_headers})
target_compile_definitions(lslboost PUBLIC BOOST_ALL_NO_LIB BOOST_THREAD_BUILD_LIB BOOST_ASIO_ENABLE_OLD_SERVICES)
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
	COMPONENT "liblsl-dev"
	ARCHIVE DESTINATION LSL/lib)
install(EXPORT lslboostConfig
	COMPONENT "liblsl-dev"
	DESTINATION LSL/cmake)
