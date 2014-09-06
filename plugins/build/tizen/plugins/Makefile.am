# Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved
#
# This file is part of Dali Toolkit
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

# Build the Dali feedback plugin

plugin_src_dir = ../../../plugins

include ../../../plugins/sounds/file.list
include ../../../plugins/file.list

plugin_sounds_dir = ../../../plugins/sounds

lib_LTLIBRARIES = libdali-feedback-plugin.la

if USE_BULLET
lib_LTLIBRARIES += libdali-bullet-plugin.la
endif

dalisounddir = ${dataReadOnlyDir}/plugins/sounds/
dalisound_DATA = ${dali_plugin_sound_files}

# Feedback
libdali_feedback_plugin_la_SOURCES = \
                     $(plugin_src_files)

libdali_feedback_plugin_la_DEPENDENCIES =

libdali_feedback_plugin_la_CXXFLAGS = -DDALI_COMPILATION \
                      -DDALI_SOUND_DIR="\"${dalisounddir}\"" \
                      $(DLOG_CFLAGS) \
                      $(MMFSOUND_CFLAGS) \
                      $(HAPTIC_CFLAGS) \
                      $(FEEDBACK_CFLAGS) \
                      -I../../.. \
                      -Werror -Wall

libdali_feedback_plugin_la_LIBADD = \
                      $(DLOG_LIBS) \
                      $(MMFSOUND_LIBS) \
                      $(HAPTIC_LIBS) \
                      $(FEEDBACK_LIBS)

libdali_feedback_plugin_la_LDFLAGS = \
                      -rdynamic

# Bullet Physics
libdali_bullet_plugin_la_SOURCES = \
                      $(bullet_plugin_src_files)

libdali_bullet_plugin_la_CXXFLAGS = -DDALI_COMPILATION \
                      $(DLOG_CFLAGS) \
                      $(BULLET_CFLAGS) \
                      -I../../.. \
                      -Werror -Wall

libdali_bullet_plugin_la_LIBADD = \
                      $(DLOG_LIBS) \
                      $(BULLET_LIBS)

libdali_bullet_plugin_la_LDFLAGS = \
                      -rdynamic

libdali_bullet_plugin_la_DEPENDENCIES =

