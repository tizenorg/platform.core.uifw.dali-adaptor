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

# Add this to the command line to get systrace enabled in the native code.
# If this is undefined, the ATRACE_BEGIN() and ATRACE_END() macros do nothing.
# BUILD_NATIVE_TRACE=1

# Setup any flags necessary to account for bugs or issues in specific compilers
# as indicated on https://developer.android.com/tools/sdk/ndk/index.html
COMPILER_VERSION=$(shell $(TARGET_CC) -v 2>&1 | tail -1 | awk '{print $$3}')
ifneq ($(findstring 4.6,$(COMPILER_VERSION)),)
$(info Compiler=4.6.x ABI=$(APP_ABI) NDK=$(NDK_TOOLCHAIN_VERSION) Platform=$(APP_PLATFORM))
else ifneq ($(findstring 4.7,$(COMPILER_VERSION)),)
$(info Compiler=4.7.x ABI=$(APP_ABI) NDK=$(NDK_TOOLCHAIN_VERSION) Platform=$(APP_PLATFORM))
HAS_ISSUE_61571=1
else ifneq ($(findstring 4.8,$(COMPILER_VERSION)),)
$(info Compiler=4.8.x ABI=$(APP_ABI) NDK=$(NDK_TOOLCHAIN_VERSION) Platform=$(APP_PLATFORM))
HAS_ISSUE_61571=1
else ifneq ($(findstring 4.9,$(COMPILER_VERSION)),)
$(info Compiler=4.9.x ABI=$(APP_ABI) NDK=$(NDK_TOOLCHAIN_VERSION) Platform=$(APP_PLATFORM))
HAS_ISSUE_61571=1

# Newer compiler complains a lot about this in boost 1.53
NO_LOCAL_TYPEDEFS=1
else
$(warning Unable to identify compiler version in $(COMPILER_VERSION))
endif



# DALI PORT STATIC LIBRARY

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../defines.mk

LIBRARY_NAME := daliport
LOCAL_MODULE := lib$(LIBRARY_NAME)

INCLUDE_PATH=$(LOCAL_PATH)/../include
LOCAL_EXPORT_C_INCLUDES += $(INCLUDE_PATH)

LOCAL_CFLAGS += -w -Wall

cppfiles :=
SRC_PATH := $(LOCAL_PATH)

public_api_src_dir=$(SRC_PATH)/dali/dali/public-api
include $(public_api_src_dir)/file.list

devel_api_src_dir = $(SRC_PATH)/dali/dali/devel-api
include $(devel_api_src_dir)/file.list

internal_src_dir=$(SRC_PATH)/dali/dali/internal
include $(internal_src_dir)/file.list

platform_abstraction_src_dir=$(SRC_PATH)/dali/dali/integration-api
include $(platform_abstraction_src_dir)/file.list

cppfiles += $(public_api_src_files) \
            $(devel_api_src_files) \
            $(internal_src_files) \
            $(platform_abstraction_src_files)

adaptor_android_dir=$(SRC_PATH)/dali-adaptor/adaptors/android
include $(adaptor_android_dir)/file.list

base_adaptor_src_dir=$(SRC_PATH)/dali-adaptor/adaptors/base
include $(base_adaptor_src_dir)/file.list

adaptor_common_dir=$(SRC_PATH)/dali-adaptor/adaptors/common
include $(adaptor_common_dir)/file.list

adaptor_public_api_dir=$(SRC_PATH)/dali-adaptor/adaptors/public-api
include $(adaptor_public_api_dir)/file.list

adaptor_devel_api_dir = $(SRC_PATH)/dali-adaptor/adaptors/devel-api
include $(adaptor_devel_api_dir)/file.list

android_platform_abstraction_src_dir=$(SRC_PATH)/dali-adaptor/platform-abstractions/android/
portable_platform_abstraction_src_dir=$(SRC_PATH)/dali-adaptor/platform-abstractions/portable
include $(android_platform_abstraction_src_dir)/file.list

cppfiles += $(adaptor_android_internal_src_files) \
            $(base_adaptor_src_files) \
            $(base_adaptor_logging_src_files) \
            $(base_adaptor_networking_src_files) \
            $(adaptor_common_internal_src_files) \
            $(adaptor_common_internal_android_src_files) \
            $(adaptor_common_internal_default_profile_src_files) \
            $(public_api_src_files) \
            $(devel_api_src_files) \
            $(platform_abstraction_src_files)

toolkit_src_dir=$(SRC_PATH)/dali-toolkit/dali-toolkit/internal
include $(toolkit_src_dir)/file.list

public_api_src_dir=$(SRC_PATH)/dali-toolkit/dali-toolkit/public-api
include $(public_api_src_dir)/file.list

devel_api_src_dir=$(SRC_PATH)/dali-toolkit/dali-toolkit/devel-api
include $(devel_api_src_dir)/file.list

cppfiles += $(toolkit_src_files) \
            $(public_api_src_files) \
            $(devel_api_src_files)

# default shaders
cppfiles += $(shell find $(SRC_PATH)/dali-generated -name '*.cpp' )

MY_LOCAL_SRC_FILES := $(cppfiles)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/, , $(MY_LOCAL_SRC_FILES))

LOCAL_STATIC_LIBRARIES := cpufeatures

LOCAL_CFLAGS += -I$(SRC_PATH)/dali
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/adaptors
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/adaptors/common
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/adaptors/android
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/adaptors/integration-api
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/adaptors/integration-api/android
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/adaptors/public-api
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/adaptors/public-api/adaptor-framework
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/adaptors/devel-api/adaptor-framework
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/adaptors/slp
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/platform-abstractions/android
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/platform-abstractions/android/resource-loader
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-adaptor/platform-abstractions/slp
LOCAL_CFLAGS += -I$(SRC_PATH)/dali/dali
LOCAL_CFLAGS += -I$(SRC_PATH)/dali/dali/integration-api
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-generated
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-toolkit
LOCAL_CFLAGS += -I$(SRC_PATH)/dali-toolkit/dali-toolkit

include $(BUILD_STATIC_LIBRARY)

# Rules for generating installable include tree and creating a tar file from it.
# Note, this target doesn't exist, so always runs as a post-install step.
# Really only want this post-install step to run if built library has changed.

all: $(LOCAL_PATH)/../libs/$(notdir $(LOCAL_BUILT_MODULE))

DALI_PUBLIC_INCLUDES := $(INCLUDE_PATH)/dali
DALI_TOOLKIT_PUBLIC_INCLUDES := $(INCLUDE_PATH)/dali-toolkit

$(LOCAL_PATH)/../libs/$(notdir $(LOCAL_BUILT_MODULE)): $(LOCAL_BUILT_MODULE)
	echo $(LOCAL_STATIC_LIBRARIES)
	mkdir -p $(DALI_PUBLIC_INCLUDES)/public-api
	mkdir -p $(DALI_PUBLIC_INCLUDES)/devel-api
	mkdir -p $(DALI_PUBLIC_INCLUDES)/integration-api/adaptors
	mkdir -p $(DALI_TOOLKIT_PUBLIC_INCLUDES)/public-api
	mkdir -p $(DALI_TOOLKIT_PUBLIC_INCLUDES)/devel-api
	(cd $(SRC_PATH)/dali/dali/public-api ; find . -name "*.h" | xargs cp --parents -t ../../../../$(DALI_PUBLIC_INCLUDES)/public-api )
	(cd $(SRC_PATH)/dali/dali/devel-api ; find . -name "*.h" | xargs cp --parents -t  ../../../../$(DALI_PUBLIC_INCLUDES)/devel-api )
	(cd $(SRC_PATH)/dali/dali/integration-api ; find . -name "*.h" | xargs cp --parents -t ../../../../$(DALI_PUBLIC_INCLUDES)/integration-api )
	cp  $(SRC_PATH)/dali-adaptor/adaptors/android/dali/dali.h $(DALI_PUBLIC_INCLUDES)
	(cd $(SRC_PATH)/dali-adaptor/adaptors/public-api ; find . -name "*.h" | xargs cp --parents -t ../../../../$(DALI_PUBLIC_INCLUDES)/public-api )
	(cd $(SRC_PATH)/dali-adaptor/adaptors/devel-api ; find . -name "*.h" | xargs cp --parents -t../../../../$(DALI_PUBLIC_INCLUDES)/devel-api )
	(cd $(SRC_PATH)/dali-adaptor/adaptors/integration-api ; find . -name "*.h" | xargs cp --parents -t ../../../../$(DALI_PUBLIC_INCLUDES)/integration-api/adaptors )
	cp    $(SRC_PATH)/dali-toolkit/dali-toolkit/dali-toolkit.h $(DALI_TOOLKIT_PUBLIC_INCLUDES)
	(cd $(SRC_PATH)/dali-toolkit/dali-toolkit/public-api ; find . -name "*.h" | xargs cp --parents -t ../../../../$(DALI_TOOLKIT_PUBLIC_INCLUDES)/public-api )
	(cd $(SRC_PATH)/dali-toolkit/dali-toolkit/devel-api ; find . -name "*.h" | xargs cp --parents -t ../../../../$(DALI_TOOLKIT_PUBLIC_INCLUDES)/devel-api )

# Don't need to generate a tarball of the include tree at this time.
#	rm -f $(LOCAL_PATH)/includes.tgz ; tar czf $(LOCAL_PATH)/includes.tgz $(LOCAL_PATH)/include
