set (Boost_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set (Boost_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (Boost_LIBRARIES boost PARENT_SCOPE)

include_directories (${Boost_INCLUDE_DIRS})

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
  add_library (boost
    ${Boost_FILES}
    src/thread/src/pthread/once.cpp
    src/thread/src/pthread/thread.cpp
  )
  set_property (TARGET boost PROPERTY COMPILE_FLAGS "-fPIC")
else ()  # WIN32
  add_library (boost
    ${Boost_FILES}
    src/thread/src/win32/thread.cpp
    src/thread/src/win32/tss_dll.cpp
    src/thread/src/win32/tss_pe.cpp
  )
  set_property (TARGET boost PROPERTY COMPILE_DEFINITIONS BOOST_ALL_NO_LIB BOOST_THREAD_BUILD_LIB)
endif (UNIX)

# gcc 5 needs -std=c++0x to compile lslboost
# CMAKE_CXX_COMPILER_VERSION is only available since CMake 2.8.8
# If CMake version is inferior to 2.8.8, we assume gcc version is inferior to 5
if (CMAKE_VERSION VERSION_GREATER 2.8.8)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
      set_property (TARGET boost PROPERTY COMPILE_FLAGS " -std=c++0x" APPEND_STRING)
    endif ()
  endif ()
endif ()
