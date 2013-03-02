# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := lslAndroid

LOCAL_SRC_FILES := ../../liblsl/src/api_config.cpp \
    ../../liblsl/src/consumer_queue.cpp \
    ../../liblsl/src/continuous_resolver.cpp \
    ../../liblsl/src/data_receiver.cpp \
    ../../liblsl/src/dllmain.cpp \
    ../../liblsl/src/freefuncs.cpp \
    ../../liblsl/src/info_receiver.cpp \
    ../../liblsl/src/inlet_connection.cpp \
    ../../liblsl/src/lsl_continuous_resolver_c.cpp \
    ../../liblsl/src/lsl_freefuncs_c.cpp \
    ../../liblsl/src/lsl_inlet_c.cpp \
    ../../liblsl/src/lsl_outlet_c.cpp \
    ../../liblsl/src/lsl_streaminfo_c.cpp \
    ../../liblsl/src/lsl_xml_element_c.cpp \
    ../../liblsl/src/resolve_attempt_udp.cpp \
    ../../liblsl/src/resolver_impl.cpp \
    ../../liblsl/src/send_buffer.cpp \
    ../../liblsl/src/stream_info.cpp \
    ../../liblsl/src/stream_info_impl.cpp \
    ../../liblsl/src/stream_inlet.cpp \
    ../../liblsl/src/stream_outlet.cpp \
    ../../liblsl/src/stream_outlet_impl.cpp \
    ../../liblsl/src/tcp_server.cpp \
    ../../liblsl/src/time_receiver.cpp \
    ../../liblsl/src/udp_server.cpp \
    ../../liblsl/src/xml_element.cpp \
    ../../liblsl/src/pugixml/pugixml.cpp \
    ../../liblsl/external/src/chrono/src/chrono.cpp \
    ../../liblsl/external/src/filesystem/src/codecvt_error_category.cpp \
    ../../liblsl/external/src/filesystem/src/operations.cpp \
    ../../liblsl/external/src/filesystem/src/path.cpp \
    ../../liblsl/external/src/filesystem/src/path_traits.cpp \
    ../../liblsl/external/src/filesystem/src/portability.cpp \
    ../../liblsl/external/src/filesystem/src/unique_path.cpp \
    ../../liblsl/external/src/filesystem/src/utf8_codecvt_facet.cpp \
    ../../liblsl/external/src/filesystem/src/windows_file_codecvt.cpp \
    ../../liblsl/external/src/serialization/src/archive_exception.cpp \
    ../../liblsl/external/src/serialization/src/basic_archive.cpp \
    ../../liblsl/external/src/serialization/src/basic_iarchive.cpp \
    ../../liblsl/external/src/serialization/src/basic_iserializer.cpp \
    ../../liblsl/external/src/serialization/src/basic_oarchive.cpp \
    ../../liblsl/external/src/serialization/src/basic_oserializer.cpp \
    ../../liblsl/external/src/serialization/src/basic_pointer_iserializer.cpp \
    ../../liblsl/external/src/serialization/src/basic_pointer_oserializer.cpp \
    ../../liblsl/external/src/serialization/src/basic_serializer_map.cpp \
    ../../liblsl/external/src/serialization/src/binary_iarchive.cpp \
    ../../liblsl/external/src/serialization/src/binary_oarchive.cpp \
    ../../liblsl/external/src/serialization/src/binary_wiarchive.cpp \
    ../../liblsl/external/src/serialization/src/binary_woarchive.cpp \
    ../../liblsl/external/src/serialization/src/codecvt_null.cpp \
    ../../liblsl/external/src/serialization/src/extended_type_info.cpp \
    ../../liblsl/external/src/serialization/src/extended_type_info_no_rtti.cpp \
    ../../liblsl/external/src/serialization/src/extended_type_info_typeid.cpp \
    ../../liblsl/external/src/serialization/src/polymorphic_iarchive.cpp \
    ../../liblsl/external/src/serialization/src/polymorphic_oarchive.cpp \
    ../../liblsl/external/src/serialization/src/shared_ptr_helper.cpp \
    ../../liblsl/external/src/serialization/src/stl_port.cpp \
    ../../liblsl/external/src/serialization/src/utf8_codecvt_facet.cpp \
    ../../liblsl/external/src/serialization/src/void_cast.cpp \
    ../../liblsl/external/src/system/src/error_code.cpp \
    ../../liblsl/external/src/thread/src/tss_null.cpp \
    ../../liblsl/external/src/thread/src/pthread/once.cpp \
    ../../liblsl/external/src/thread/src/pthread/thread.cpp \
    liblsl_wrap_java.cxx

LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_CPP_EXTENSION := .cxx .cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../liblsl/include $(LOCAL_PATH)/../../liblsl/external

include $(BUILD_SHARED_LIBRARY)
