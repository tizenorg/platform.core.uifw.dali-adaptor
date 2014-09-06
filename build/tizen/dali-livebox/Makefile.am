# Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved
#
# This file is part of Dali Adaptor
#
# PROPRIETARY/CONFIDENTIAL
#
# This software is the confidential and proprietary information of
# SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
# disclose such Confidential Information and shall use it only in
# accordance with the terms of the license agreement you entered
# into with SAMSUNG ELECTRONICS.
#
# SAMSUNG make no representations or warranties about the suitability
# of the software, either express or implied, including but not limited
# to the implied warranties of merchantability, fitness for a particular
# purpose, or non-infringement. SAMSUNG shall not be liable for any
# damages suffered by licensee as a result of using, modifying or
# distributing this software or its derivatives.

# Build the Dali Adaptor + Livebox Plugin library

tizen_application_internal_src_dir = ../../../adaptors/tizen/internal
include ../../../adaptors/tizen/internal/application-file.list

tizen_livebox_plugin_public_api_src_dir = ../../../adaptors/tizen/public-api
include ../../../adaptors/tizen/public-api/adaptor-framework/application-file.list

lib_LTLIBRARIES = libdali-livebox-plugin.la

libdali_livebox_plugin_la_SOURCES = \
                     $(tizen_livebox_plugin_public_api_src_files) \
                     $(tizen_livebox_plugin_internal_src_files)

libdali_livebox_plugin_la_DEPENDENCIES =

libdali_livebox_plugin_includes = \
                      -I../../.. \
                      -I../../../platform-abstractions/ \
                      -I../../../platform-abstractions/slp \
                      -I../../../platform-abstractions/slp/resource-loader \
                      -I../../../adaptors/ \
                      -I../../../adaptors/tizen \
                      -I../../../adaptors/tizen\common

daliDefaultFontCacheDir = ${dataReadOnlyDir}/glyphcache/
daliUserFontCacheDir = ${dataReadWriteDir}/glyphcache/
daliDefaultThemeDir  = ${dataReadWriteDir}/theme/

libdali_livebox_plugin_la_CXXFLAGS = \
                      -DDALI_DATA_RW_DIR="\"${daliReadWriteDir}\"" \
                      -DDALI_DATA_RO_DIR="\"${daliReadOnlyDir}\"" \
                      -DDALI_DEFAULT_FONT_CACHE_DIR="\"${daliDefaultFontCacheDir}\"" \
                      -DDALI_USER_FONT_CACHE_DIR="\"${daliUserFontCacheDir}\"" \
                      -DDALI_DEFAULT_THEME_DIR="\"${daliDefaultThemeDir}\"" \
                      -DFONT_PRELOADED_PATH="\"${fontPreloadedPath}\"" \
                      -DFONT_DOWNLOADED_PATH="\"${fontDownloadedPath}\"" \
                      -DFONT_CONFIGURATION_FILE_PATH="\"${fontConfigurationFilePath}\"" \
                      -DNON_POWER_OF_TWO_TEXTURES \
                      -DDALI_COMPILATION -DDALI_ADAPTOR_COMPILATION \
                      -Werror -Wall \
                      $(libdali_livebox_plugin_includes) \
                      $(DALI_ADAPTOR_CFLAGS) \
                      $(DALICORE_CFLAGS) \
                      $(OPENGLES20_CFLAGS) \
                      $(X11_CFLAGS) \
                      $(FREETYPE_CFLAGS) \
                      $(FONTCONFIG_CFLAGS) \
                      $(PNG_CFLAGS) \
                      $(ASSIMP_CFLAGS) \
                      $(CAPI_APPFW_APPLICATION_CFLAGS) \
                      $(ELEMENTARY_CFLAGS) \
                      $(ECORE_CFLAGS) \
                      $(DLOG_CFLAGS) \
                      $(XML_CFLAGS) \
                      $(VCONF_CFLAGS) \
                      $(EXIF_CFLAGS) \
                      $(MMFSOUND_CFLAGS) \
                      $(HAPTIC_CFLAGS) \
                      $(TTS_CFLAGS) \
                      $(LIVEBOX_CLIENT_CFLAGS) \
                      $(LIVEBOX_SERVICE_CFLAGS)


libdali_livebox_plugin_la_LIBADD = \
                      $(CAPI_APPFW_APPLICATION_LIBS) \
                      $(LIVEBOX_CLIENT_LIBS) \
                      $(LIVEBOX_SERVICE_LIBS)

tizenadaptorpublicapidir = $(includedir)/dali/public-api/adaptor-framework
tizenadaptorpublicapi_HEADERS = $(tizen_livebox_plugin_public_api_header_files)

# linking test

noinst_PROGRAMS = linker.test

linker_test_SOURCES = linker-test.cpp

linker_test_CXXFLAGS = \
  -I../../../adaptors/tizen \
  $(DALI_ADAPTOR_CFLAGS) \
  $(DALICORE_CFLAGS) \
  $(DALIX11_CFLAGS) \
  $(LIVEBOX_CLIENT_CFLAGS) \
  $(LIVEBOX_SERVICE_CFLAGS) \
  -Werror -Wall

linker_test_DEPENDENCIES = libdali-livebox-plugin.la
linker_test_LDADD = \
  $(DALICORE_LIBS) \
  libdali-livebox-plugin.la \
  ../common/libdali-adaptor-common.la \
  -L${prefix}/lib
