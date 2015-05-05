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
  time_receiver.cpp
  udp_server.cpp
  pugixml/pugixml.cpp
  legacy/legacy_abi.cpp
)

message (${Boost_LIBRARIES})

if (BUILD_SHARED)
  add_library (lsl SHARED ${sources})
  
  # TODO: Need to check if all flags are necessary or if some flags are missing
  if (WIN32)
    set_property (TARGET lsl PROPERTY COMPILE_DEFINITIONS BOOST_ALL_NO_LIB BOOST_THREAD_BUILD_LIB LIBLSL_EXPORTS _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS)
    target_link_libraries (lsl ${Boost_LIBRARIES})
  elseif (APPLE)
    target_link_libraries (lsl ${Boost_LIBRARIES} pthread)
  elseif (UNIX)
    set_property (TARGET lsl PROPERTY COMPILE_FLAGS "-fPIC -Wl,--exclude-libs,ALL")
    target_link_libraries (lsl ${Boost_LIBRARIES} rt pthread)
  else ()
    target_link_libraries (lsl ${Boost_LIBRARIES})
  endif ()
endif (BUILD_SHARED)

if (BUILD_STATIC)
  add_library (lsl-static STATIC ${sources})
  
  # TODO: Need to check if all flags are necessary or if some flags are missing
  # TODO: Add support for other platforms
  if (UNIX)
    set_property (TARGET lsl-static PROPERTY COMPILE_FLAGS "-fPIC")
    set_target_properties (lsl-static PROPERTIES OUTPUT_NAME lsl)
    target_link_libraries (lsl-static ${Boost_LIBRARIES} rt pthread)
  endif ()
endif (BUILD_STATIC)


