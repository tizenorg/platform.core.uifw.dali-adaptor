# Pre-processor defines
# ---------------------

# 3rd party defines
LOCAL_CFLAGS += -DASSIMP_BUILD_NO_OWN_ZLIB                        # Needed by assimp
LOCAL_CFLAGS += -DGETTEXT_PACKAGE=\"libexif2\"                    # Needed by libexif build
LOCAL_CFLAGS += -DFT_OPTION_AUTOFIT2                              # Needed by freetype
# LOCAL_CFLAGS += -DFC_CACHEDIR="\"/fc-cachedir\""                # Fontconfig is presently unused due to large startup times
# LOCAL_CFLAGS += -DFONTCONFIG_PATH="\"/fc-config-path\""         # Fontconfig is presently unused due to large startup times
LOCAL_CFLAGS += -D_GLIBCXX_PERMIT_BACKWARD_HASH                   # Used in GCC STL hash-map implementation

# Dali defines
LOCAL_CFLAGS += -DDALI_SHADERBIN_DIR="\"/data/data/com.sec.android.app.launcher/cache/\""             # Used in shader-factory.cpp
LOCAL_CFLAGS += -DDALI_GLYPH_CACHE_DIR="\"/data/data/com.sec.android.app.launcher/cache/\""           # dali/internal/event/text/font*
LOCAL_CFLAGS += -DDALI_DEFAULT_FONT_CACHE_DIR="\"/data/data/com.sec.android.app.launcher/cache/\""    # Not used in current resource-thread-text.cpp
LOCAL_CFLAGS += -DDALI_EMOTICON_DIR="\"/data/data/com.sec.android.app.launcher/cache/\""              # Emoticon dir (NEEDS to verify what this is)y
LOCAL_CFLAGS += -DDALI_USER_FONT_CACHE_DIR="\"/data/data/com.sec.android.app.launcher/cache/\""       # Used in platform-abstractions/android/resource-loader/resource-thread-text.cpp
LOCAL_CFLAGS += -DDALI_FEEDBACK_THEME_DIR="\"/data/data/com.sec.android.app.launcher/cache/\""
LOCAL_CFLAGS += -DFONT_CONFIGURATION_FILE_PATH="\"/data/data/com.sec.android.app.launcher/cache/fonts.conf\""     # Used in platform-abstractions/android/slp-platform-abstraction.cpp
LOCAL_CFLAGS += -DFONT_PRELOADED_PATH="\"/data/data/com.sec.android.app.launcher/cache/\""            # Used in platform-abstractions/android/font-platform/font-controller-impl.cpp
LOCAL_CFLAGS += -DFONT_DOWNLOADED_PATH="\"/data/data/com.sec.android.app.launcher/cache/\""           # Used in platform-abstractions/android/font-platform/font-controller-impl.cpp
LOCAL_CFLAGS += -DNON_POWER_OF_TWO_TEXTURES
LOCAL_CFLAGS += -DDALI_IMAGE_DIR="\"\""                           # Used by test code and dali-toolkit for image resources
LOCAL_CFLAGS += -DDALI_STYLE_IMAGE_DIR="\"\""
LOCAL_CFLAGS += -DDALI_DATA_READ_ONLY_DIR="\"\""
LOCAL_CFLAGS += -DDALI_STYLE_DIR="\"/data/data/com.sec.android.app.launcher/cache/\""
LOCAL_CFLAGS += -DDALI_SOUND_DIR="\"\""
LOCAL_CFLAGS += -DDALI_GLES_VERSION=20
LOCAL_CFLAGS += -DNETWORK_LOGGING_ENABLED

# Warnings, optimisation and debug flags
# --------------------------------------
LOCAL_CFLAGS += -g
ifeq ($(APP_OPTIM),debug)
#LOCAL_CFLAGS += -DDALI_GL_ERROR_CHECK                             # Extra GL error checking / reporting in Dali renderer
LOCAL_CFLAGS += -DDEBUG_ENABLED                                   # Generic flag to enable additional debug in code (toolkit, adaptor, dali, and also libxml)
LOCAL_CFLAGS += -O0
ifeq ($(BUILD_NATIVE_TRACE), 1)
LOCAL_CFLAGS += -DBUILD_NATIVE_TRACE
$(info Native Android systrace enabled)
endif
else
LOCAL_CFLAGS += -Os
endif

ifeq ($(DALI_LOGGING), 1)
LOCAL_CFLAGS += -DENABLE_DALI_LOGGING
endif

# This flag is only needed if you're attempting a boost header file cleanup
# See dependencies/SO_A_BOOST_FILE_IS_MISSING_README.txt
# NOTE: Adding this flag breaks the actual compilation process, but for
# the purposes of updating boost dependencies, it doesn't matter.
# LOCAL_CFLAGS += -M

ifeq ($(HAS_ISSUE_61571),1)
# Introduced in NDK, Revision 10 (July 2014)
LOCAL_CFLAGS += -fno-strict-aliasing
$(info Strict Aliasing Turned Off)
endif
ifeq ($(NO_LOCAL_TYPEDEFS),1)
LOCAL_CFLAGS += -Wno-unused-local-typedefs
$(info No warnings for unused local typedefs)
endif

# C++ language features
# ---------------------
LOCAL_CPPFLAGS += -fexceptions
LOCAL_CPPFLAGS += -frtti

# Linker options and search paths
# -------------------------------
#LOCAL_LDFLAGS += -L$(PLATFORM_PREFIX)/homescreen-lib
#LOCAL_LDFLAGS += -L$(NDK_ROOT)/sources/boost/lib
#LOCAL_LDFLAGS += -L$(TARGET_OUT) -Wl,--version-script=$(LOCAL_PATH)/nativeDali.export
