cmake_minimum_required (VERSION 2.6)
project (liblsl)

# Version number
set (liblsl_VERSION_MAJOR 1)
set (liblsl_VERSION_MINOR 3)
set (liblsl_VERSION_PATCH 2)

option (USE_SYSTEM_BOOST "Use system Boost libraries instead of the version shipped with LSL." OFF)

if (USE_SYSTEM_BOOST)
  find_package(Boost COMPONENTS chrono filesystem serialization system thread REQUIRED)
else ()
  set (Boost_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/external)
  set (Boost_LIBRARIES boost)
  include_directories (${Boost_INCLUDE_DIRS})
  add_library (boost
    external/src/chrono/chrono.cpp
    external/src/filesystem/codecvt_error_category.cpp
    external/src/filesystem/operations.cpp
    external/src/filesystem/path.cpp
    external/src/filesystem/path_traits.cpp
    external/src/filesystem/portability.cpp
    external/src/filesystem/unique_path.cpp
    external/src/filesystem/utf8_codecvt_facet.cpp
    external/src/filesystem/windows_file_codecvt.cpp
    external/src/filesystem/windows_file_codecvt.hpp
    external/src/serialization/archive_exception.cpp
    external/src/serialization/basic_archive.cpp
    external/src/serialization/basic_iarchive.cpp
    external/src/serialization/basic_iserializer.cpp
    external/src/serialization/basic_oarchive.cpp
    external/src/serialization/basic_oserializer.cpp
    external/src/serialization/basic_pointer_iserializer.cpp
    external/src/serialization/basic_pointer_oserializer.cpp
    external/src/serialization/basic_serializer_map.cpp
    external/src/serialization/binary_iarchive.cpp
    external/src/serialization/binary_oarchive.cpp
    external/src/serialization/binary_wiarchive.cpp
    external/src/serialization/binary_woarchive.cpp
    external/src/serialization/codecvt_null.cpp
    external/src/serialization/extended_type_info.cpp
    external/src/serialization/extended_type_info_no_rtti.cpp
    external/src/serialization/extended_type_info_typeid.cpp
    external/src/serialization/polymorphic_iarchive.cpp
    external/src/serialization/polymorphic_oarchive.cpp
    external/src/serialization/shared_ptr_helper.cpp
    external/src/serialization/stl_port.cpp
    external/src/serialization/utf8_codecvt_facet.cpp
    external/src/serialization/void_cast.cpp
    external/src/system/error_code.cpp
    external/src/system/local_free_on_destruction.hpp
    external/src/thread/pthread/once.cpp
    external/src/thread/pthread/thread.cpp
    external/src/thread/tss_null.cpp
    )
    if (NOT MSVC)
      set_property (TARGET boost PROPERTY COMPILE_FLAGS "-fPIC")
    endif ()
endif (USE_SYSTEM_BOOST)

add_subdirectory (src)
