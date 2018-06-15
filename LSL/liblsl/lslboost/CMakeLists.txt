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
# generate via grep " <lslboost" -hr .| sed -e 's/#include <lsl//;s/>$//' | sort | uniq
set(lslboost_headers
	boost/algorithm/string.hpp
	boost/asio.hpp
	boost/asio/basic_socket.hpp
	boost/asio/deadline_timer.hpp
	boost/asio/detail/array.hpp
	boost/asio/detail/config.hpp
	boost/asio/detail/throw_error.hpp
	boost/asio/io_service.hpp
	boost/asio/ip/tcp.hpp
	boost/asio/ip/udp.hpp
	boost/asio/stream_socket_service.hpp
	boost/asio/streambuf.hpp
	boost/atomic.hpp
	boost/bimap.hpp
	boost/bind.hpp
	boost/chrono/system_clocks.hpp
	boost/container/flat_set.hpp
	boost/cstdint.hpp
	boost/date_time/time_duration.hpp
	boost/enable_shared_from_this.hpp
	boost/function.hpp
	boost/functional/hash.hpp
	boost/intrusive_ptr.hpp
	boost/lexical_cast.hpp
	boost/lockfree/spsc_queue.hpp
	boost/noncopyable.hpp
	boost/property_tree/ini_parser.hpp
	boost/property_tree/ptree.hpp
	boost/random.hpp
	boost/scoped_ptr.hpp
	boost/serialization/split_member.hpp
	boost/serialization/string.hpp
	boost/shared_ptr.hpp
	boost/smart_ptr.hpp
	boost/static_assert.hpp
	boost/system/system_error.hpp
	boost/thread.hpp
	boost/thread/condition_variable.hpp
	boost/thread/mutex.hpp
	boost/thread/once.hpp
	boost/thread/shared_mutex.hpp
	boost/thread/thread.hpp
	boost/type_traits.hpp
	boost/utility/base_from_member.hpp
	boost/uuid/uuid.hpp
	boost/uuid/uuid_generators.hpp
	boost/uuid/uuid_io.hpp
	boost/version.hpp
)
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
target_compile_definitions(lslboost PUBLIC
	BOOST_ALL_NO_LIB
	BOOST_THREAD_BUILD_LIB
	BOOST_ASIO_ENABLE_OLD_SERVICES
)
target_include_directories(lslboost SYSTEM PUBLIC
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

export(TARGETS lslboost
	FILE lslboostConfig.cmake)
