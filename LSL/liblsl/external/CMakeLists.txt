set (Boost_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set (Boost_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (Boost_LIBRARIES boost PARENT_SCOPE)

include_directories (${Boost_INCLUDE_DIRS})

set (Boost_FILES  # Platform-independent sources
  src/chrono/chrono.cpp
  src/filesystem/codecvt_error_category.cpp
  src/filesystem/operations.cpp
  src/filesystem/path.cpp
  src/filesystem/path_traits.cpp
  src/filesystem/portability.cpp
  src/filesystem/unique_path.cpp
  src/filesystem/utf8_codecvt_facet.cpp
  src/filesystem/windows_file_codecvt.cpp
  src/filesystem/windows_file_codecvt.hpp
  src/serialization/archive_exception.cpp
  src/serialization/basic_archive.cpp
  src/serialization/basic_iarchive.cpp
  src/serialization/basic_iserializer.cpp
  src/serialization/basic_oarchive.cpp
  src/serialization/basic_oserializer.cpp
  src/serialization/basic_pointer_iserializer.cpp
  src/serialization/basic_pointer_oserializer.cpp
  src/serialization/basic_serializer_map.cpp
  src/serialization/binary_iarchive.cpp
  src/serialization/binary_oarchive.cpp
  src/serialization/binary_wiarchive.cpp
  src/serialization/binary_woarchive.cpp
  src/serialization/codecvt_null.cpp
  src/serialization/extended_type_info.cpp
  src/serialization/extended_type_info_no_rtti.cpp
  src/serialization/extended_type_info_typeid.cpp
  src/serialization/polymorphic_iarchive.cpp
  src/serialization/polymorphic_oarchive.cpp
  src/serialization/shared_ptr_helper.cpp
  src/serialization/stl_port.cpp
  src/serialization/utf8_codecvt_facet.cpp
  src/serialization/void_cast.cpp
  src/system/error_code.cpp
  src/system/local_free_on_destruction.hpp
  src/thread/tss_null.cpp
)
if (UNIX)
  add_library (boost
    ${Boost_FILES}
    src/thread/pthread/once.cpp
    src/thread/pthread/thread.cpp
  )
  set_property (TARGET boost PROPERTY COMPILE_FLAGS "-fPIC")
else ()  # WIN32
  add_library (boost
    ${Boost_FILES}
    src/thread/win32/thread.cpp
    src/thread/win32/tss_dll.cpp
    src/thread/win32/tss_pe.cpp
  )
  set_property (TARGET boost PROPERTY COMPILE_DEFINITIONS BOOST_ALL_NO_LIB BOOST_THREAD_BUILD_LIB)
endif (UNIX)
