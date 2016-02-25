#
# Copyright (c) 2015 Samsung Electronics Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

m4_define([dali_version],[0.1.0])
AC_INIT([dali], [dali_version])
AM_INIT_AUTOMAKE([-Wall foreign])

AC_ARG_WITH([libuv],
              [AC_HELP_STRING([--with-libuv],
                              [Path that contains libuv headers. Setting this configures DALi to work with LibUV mainloop used in Node.JS.
                              For example /usr/tmp/downloads/node/deps/uv/include/ ])],
              [with_libuv=$withval],
              [with_libuv=no])

AC_ARG_ENABLE([feedback],
              [AC_HELP_STRING([ --enable-feedback],
                              [Enable feedback plugin])],
              [enable_feedback=yes],
              [enable_feedback=no])

AC_CONFIG_SUBDIRS(adaptor)
if test "x$with_libuv" != "xno"; then
  # build dali-adaptor & dali-adaptor-uv
  AC_CONFIG_SUBDIRS(adaptor-uv)
fi

AC_ARG_ENABLE([appfw],
              [AC_HELP_STRING([--enable-appfw],
                              [Builds with Tizen App framework libraries, off by default])],
              [enable_appfw=$enableval],
              [enable_appfw=no])

# Option to allow building with Tizen SDK 2.2
AC_ARG_WITH([tizen-2-2-compatibility],
            [AC_HELP_STRING([--with-tizen-2-2-compatibility],
                            [Use Tizen SDK 2.2 compatibility])],
            [with_tizen_2_2_compatibility=$withval],
            [with_tizen_2_2_compatibility=no])

# Tizen Profile options
AC_ARG_ENABLE([profile],
              [AC_HELP_STRING([--enable-profile=COMMON,MOBILE,WEARABLE,TV,UBUNTU],
                            [Select the variant of tizen])],
              [enable_profile=$enableval],
              [enable_profile=COMMON])

# Ensure valid profile selected
if test "x$enable_profile" != "xCOMMON" -a "x$enable_profile" != "xMOBILE" -a "x$enable_profile" != "xWEARABLE" -a "x$enable_profile" != "xTV" -a "x$enable_profile" != "xUBUNTU"; then
  AC_MSG_ERROR([$enable_profile is an invalid profile])
fi

AC_ARG_ENABLE(wayland,
              [  --enable-wayland       Build on Wayland],
              enable_wayland=yes,
              enable_wayland=no)

DALI_ADAPTOR_CFLAGS="$DALI_ADAPTOR_CFLAGS -DDALI_PROFILE_${enable_profile}"
DALI_PROFILE_CFLAGS=" -DDALI_PROFILE_${enable_profile}"
AM_CONDITIONAL([COMMON_PROFILE], [test x$enable_profile = xCOMMON])
AM_CONDITIONAL([MOBILE_PROFILE], [test x$enable_profile = xMOBILE])
AM_CONDITIONAL([WEARABLE_PROFILE], [test x$enable_profile = xWEARABLE])
AM_CONDITIONAL([TV_PROFILE], [test x$enable_profile = xTV])
AM_CONDITIONAL([UBUNTU_PROFILE], [test x$enable_profile = xUBUNTU])
AM_CONDITIONAL([WAYLAND], [test x$enable_wayland = xyes])
AM_CONDITIONAL([USE_EFL], [test x$enable_efl = xyes])
AM_CONDITIONAL([USE_APPFW], [test x$enable_appfw = xyes])

AM_CONDITIONAL([ENABLE_NETWORK_LOGGING], [test x$enable_networklogging = xyes])

# Platforms should either enable features or remove them, they
# should not disable features. This allows the developer to override
# features through the command line.

if test "x$enable_profile" = "xCOMMON"; then
PKG_CHECK_MODULES(OPENGLES20, glesv2 egl)
fi

if test "x$enable_profile" = "xMOBILE"; then
PKG_CHECK_MODULES(OPENGLES20, gles20)
enable_assimp=no
fi

if test "x$enable_profile" = "xLITE"; then
PKG_CHECK_MODULES(OPENGLES20, gles20)
fi

if test "x$enable_profile" = "xWEARABLE"; then
PKG_CHECK_MODULES(OPENGLES20, glesv2)
fi

if test "x$enable_profile" = "xTV"; then
PKG_CHECK_MODULES(OPENGLES20, glesv2)
fi

if test "x$enable_profile" = "xUBUNTU"; then
PKG_CHECK_MODULES(OPENGLES20, glesv2 egl)
else



PKG_CHECK_MODULES(DLOG, dlog)
PKG_CHECK_MODULES(SENSOR, sensor)
PKG_CHECK_MODULES(TTS, tts)
PKG_CHECK_MODULES(VCONF, vconf)
if test "x$enable_efl" = "xyes"; then
if test "x$with_tizen_2_2_compatibility" = "xno"; then
PKG_CHECK_MODULES(CAPI_SYSTEM_INFO, capi-system-info)
fi
fi

fi # ubuntu profile test

if test "x$enable_appfw" = "xyes"; then
PKG_CHECK_MODULES(CAPI_APPFW_APPLICATION, capi-appfw-application)
PKG_CHECK_MODULES(CAPI_SYSTEM_SYSTEM_SETTINGS, capi-system-system-settings)
fi

# Using EFL api's for  WAYLAND AND X11 to run on ecore mainloop
if test "x$enable_efl" = "xyes"; then
if test "x$enable_wayland" = "xyes"; then
PKG_CHECK_MODULES(WAYLAND, [ecore-wayland egl wayland-egl wayland-client >= 1.2.0 xkbcommon],
                  [DALI_USE_ECORE_WAYLAND=1],
                  [DALI_USE_ECORE_WAYLAND=0])

else
PKG_CHECK_MODULES(ECORE_X, [ecore-x],
                  [DALI_USE_ECORE_X11=1],
                  [DALI_USE_ECORE_X11=0])
PKG_CHECK_MODULES(X11, [x11],
                  [DALI_USE_X11=1],
                  [DALI_USE_X11=0])
PKG_CHECK_MODULES(UTILX, utilX)
fi
fi
# Using Wayland API directly  ( main loop agnostic, typically for running on libuv)
#  wayland-extension-client include xdg-shell-client
if test "x$enable_efl" = "xno"; then
if test "x$enable_wayland" = "xyes"; then
PKG_CHECK_MODULES(WAYLAND, [ egl wayland-egl wayland-client >= 1.2.0 xkbcommon],
                  [DALI_USE_WAYLAND=1],
                  [DALI_USE_WAYLAND=0])
fi
fi

# remove this when we update common repos
# common profile currently does not have wayland extensions like xdg-shell
if test "x$enable_wayland" = "xyes"; then
if test "x$enable_profile" != "xCOMMON"; then
PKG_CHECK_MODULES(WAYLAND_EXTENSION, xdg-shell-client)
fi
fi
AM_CONDITIONAL([USE_ECORE_WAYLAND], [test "$DALI_USE_ECORE_WAYLAND" -eq 1])

if test "x$enable_feedback" = "xyes"; then
  # build dali-adaptor & dali-adaptor-uv & plugins
  AC_CONFIG_SUBDIRS(plugins)
fi
#else
  # build dali-adaptor only

AC_CONFIG_FILES(Makefile)

AC_OUTPUT
