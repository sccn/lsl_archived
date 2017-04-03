include_directories (${Boost_INCLUDE_DIRS})

set (sources
  api_config.cpp
  common.cpp
  consumer_queue.cpp
  data_receiver.cpp
  dllmain.cpp
  info_receiver.cpp
  inlet_connection.cpp
  lsl_continuous_resolver_c.cpp
  lsl_freefuncs_c.cpp
  lsl_inlet_c.cpp
  lsl_outlet_c.cpp
  lsl_streaminfo_c.cpp
  lsl_xml_element_c.cpp
  resolve_attempt_udp.cpp
  resolver_impl.cpp
  sample.cpp
  send_buffer.cpp
  socket_utils.cpp
  stream_info_impl.cpp
  stream_outlet_impl.cpp
  tcp_server.cpp
  time_postprocessor.cpp
  time_receiver.cpp
  udp_server.cpp
  pugixml/pugixml.cpp
  legacy/legacy_abi.cpp
)

message (${Boost_LIBRARIES})

set(target lsl64)
if (NOT (CMAKE_SIZEOF_VOID_P EQUAL 8))
  set(target lsl32)
endif()

if (BUILD_SHARED)
  add_library (${target} SHARED ${sources})
  target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../external)
  target_link_libraries (${target} ${Boost_LIBRARIES})
  set_target_properties(${target}
    PROPERTIES
        VERSION ${liblsl_VERSION_MAJOR}.${liblsl_VERSION_MINOR}.${liblsl_VERSION_PATCH}
        SOVERSION ${liblsl_VERSION_MAJOR}.0.0
  )

  # TODO: Need to check if all flags are necessary or if some flags are missing
  if (WIN32)
    if (MINGW)
      target_link_libraries (${target} winmm ws2_32 wsock32)
    endif ()
    set_property(
        TARGET ${target}
        PROPERTY
            COMPILE_DEFINITIONS
                BOOST_ALL_NO_LIB
                BOOST_THREAD_BUILD_LIB
                LIBLSL_EXPORTS
                _SCL_SECURE_NO_WARNINGS
                _CRT_SECURE_NO_WARNINGS
    )
    set_target_properties(${target}
        PROPERTIES
            PREFIX lib
            IMPORT_PREFIX lib
    )
  elseif (APPLE)
    target_link_libraries (${target} pthread)
  elseif (UNIX)
    set_property (TARGET ${target} PROPERTY COMPILE_FLAGS "-fPIC -Wl,--exclude-libs,ALL")
    target_link_libraries (${target} rt pthread)
  endif ()

  if (CMAKE_VERSION VERSION_GREATER 2.8.8)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
        set_property (TARGET ${target} PROPERTY COMPILE_FLAGS " -std=c++0x" APPEND_STRING)
      endif ()
    endif ()
  endif()

endif (BUILD_SHARED)

if (BUILD_STATIC)
  add_library (${target}-static STATIC ${sources})
  target_include_directories(${target}-static PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../external)
  
  set_property (TARGET ${target}-static PROPERTY COMPILE_DEFINITIONS LIBLSL_STATIC)
  if(WIN32)
    set_target_properties(${target}-static
            PROPERTIES
                PREFIX lib
                IMPORT_PREFIX lib
    )
  endif()

  # TODO: Need to check if all flags are necessary or if some flags are missing
  # TODO: Add support for other platforms
  if (UNIX)
    set_property (TARGET ${target}-static PROPERTY COMPILE_FLAGS "-fPIC")
    set_target_properties (${target}-static PROPERTIES OUTPUT_NAME ${target})
    target_link_libraries (${target}-static ${Boost_LIBRARIES} rt pthread)
  endif ()

  if (CMAKE_VERSION VERSION_GREATER 2.8.8)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
        set_property (TARGET ${target}-static PROPERTY COMPILE_FLAGS " -std=c++0x" APPEND_STRING)
      endif ()
    endif ()
  endif()

endif (BUILD_STATIC)


