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

FILE_LIST := $(wildcard $(LOCAL_PATH)/../../liblsl/src/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../liblsl/src/pugixml/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../liblsl/external/src/chrono/src/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../liblsl/external/src/filesystem/src/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../liblsl/external/src/serialization/src/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../liblsl/external/src/system/src/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../liblsl/external/src/thread/src/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../liblsl/external/src/thread/src/pthread/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/liblsl_wrap_java.cxx)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)


LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_CPP_EXTENSION := .cxx .cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../liblsl/include $(LOCAL_PATH)/../../liblsl/external

include $(BUILD_SHARED_LIBRARY)
