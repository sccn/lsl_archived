set (Boost_FILES  # Platform-independent sources
  src/chrono/src/chrono.cpp
  src/filesystem/src/codecvt_error_category.cpp
  src/filesystem/src/operations.cpp
  src/filesystem/src/path.cpp
  src/filesystem/src/path_traits.cpp
  src/filesystem/src/portability.cpp
  src/filesystem/src/unique_path.cpp
  src/filesystem/src/utf8_codecvt_facet.cpp
  src/filesystem/src/windows_file_codecvt.cpp
  src/filesystem/src/windows_file_codecvt.hpp
  src/serialization/src/archive_exception.cpp
  src/serialization/src/basic_archive.cpp
  src/serialization/src/basic_iarchive.cpp
  src/serialization/src/basic_iserializer.cpp
  src/serialization/src/basic_oarchive.cpp
  src/serialization/src/basic_oserializer.cpp
  src/serialization/src/basic_pointer_iserializer.cpp
  src/serialization/src/basic_pointer_oserializer.cpp
  src/serialization/src/basic_serializer_map.cpp
  src/serialization/src/binary_iarchive.cpp
  src/serialization/src/binary_oarchive.cpp
  src/serialization/src/binary_wiarchive.cpp
  src/serialization/src/binary_woarchive.cpp
  src/serialization/src/codecvt_null.cpp
  src/serialization/src/extended_type_info.cpp
  src/serialization/src/extended_type_info_no_rtti.cpp
  src/serialization/src/extended_type_info_typeid.cpp
  src/serialization/src/polymorphic_iarchive.cpp
  src/serialization/src/polymorphic_oarchive.cpp
  src/serialization/src/shared_ptr_helper.cpp
  src/serialization/src/stl_port.cpp
  src/serialization/src/utf8_codecvt_facet.cpp
  src/serialization/src/void_cast.cpp
  src/system/src/error_code.cpp
  src/system/src/local_free_on_destruction.hpp
  src/thread/src/tss_null.cpp
)
if (UNIX)
  list(APPEND Boost_FILES
	src/thread/src/pthread/once.cpp
	src/thread/src/pthread/thread.cpp
  )
  set(Boost_LIBRARIES pthread)
else ()  # WIN32
  list(APPEND Boost_FILES
	src/thread/src/win32/thread.cpp
	src/thread/src/win32/tss_dll.cpp
	src/thread/src/win32/tss_pe.cpp
  )
  set(Boost_LIBRARIES "")
endif ()

add_library (lslboost ${Boost_FILES})
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
	COMPONENT "liblsl-dev"
  ARCHIVE DESTINATION LSL/lib
  LIBRARY DESTINATION LSL/lib)
#install(DIRECTORY boost lslboost
#	DESTINATION LSL/include)
install(EXPORT lslboostConfig
	COMPONENT "liblsl-dev"
  DESTINATION LSL/cmake)
