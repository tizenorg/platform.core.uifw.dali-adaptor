cmake_minimum_required(VERSION 2.6)

if( ${EMSCRIPTEN} )
  message("EMSCRIPTEN BUILD")

  set(CMAKE_C_COMPILER "emcc")
  set(CMAKE_CXX_COMPILER "em++")
else( ${EMSCRIPTEN} )
  message( FATAL_ERROR "Native Build not supported via cmake." )

endif( ${EMSCRIPTEN} )

project(dali-emscripten CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(SRCS
  ${CMAKE_SOURCE_DIR}/../../adaptors/base/frame-time.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/common/gl/egl-image-extensions.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/common/gl/gl-extensions.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/emscripten/egl-implementation-emscripten.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/emscripten/main.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/emscripten/sdl-application.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/emscripten/sdl-gl-sync-abstraction.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/actor-wrapper.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/animation-wrapper.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/dali-wrapper.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/emscripten-callbacks.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/emscripten-callbacks.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/emscripten-utils.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/handle-wrapper.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/image-wrapper.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/property-buffer-wrapper.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/property-value-wrapper.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/render-task-wrapper.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/shader-effect-wrapper.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/sdl-platform-abstraction.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/sdl-platform-abstraction.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/type-info-wrapper.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/../../)
include_directories(${CMAKE_SOURCE_DIR}/../../adaptors/)
include_directories(${CMAKE_SOURCE_DIR}/../../adaptors/common)
include_directories(${CMAKE_SOURCE_DIR}/../../adaptors/tizen)
include_directories(${CMAKE_SOURCE_DIR}/../../adaptors/emscripten)
include_directories(${CMAKE_SOURCE_DIR}/../../platform-abstractions/slp)

configure_file(${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/dali-wrapper.js dali-wrapper.js COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/dali-toy.js dali-toy.js COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/dali-toy.html dali-toy.html COPYONLY)

set(VENDOR "samsung")
set(PACKAGE ${PROJECT_NAME})
set(DESKTOP_PREFIX $ENV{DESKTOP_PREFIX})
set(CPP_DEFINES -DDALI_GLES_VERSION=2)

set(DEBUG_FLAGS "-Wall -g -O1")
set(RELEASE_FLAGS "-Wall -g -O1")

if( ${EMSCRIPTEN} )
 set(DEBUG_FLAGS "${DEBUG_FLAGS} -std=c++11 -DBOOST_ERROR_CODE_HEADER_ONLY")
 set(RELEASE_FLAGS "${RELEASE_FLAGS} -std=c++11 -DBOOST_ERROR_CODE_HEADER_ONLY")
endif( ${EMSCRIPTEN} )


include(FindPkgConfig)

pkg_check_modules(pkgs REQUIRED
  dali-core
  # sdl from emscripten
  )

if( ${EMSCRIPTEN} )

pkg_check_modules(pkgs REQUIRED
  dali-core
  # sdl from emscripten
  )

else( ${EMSCRIPTEN} )

find_library(SDL VERSION "1.2" REQUIRED HINTS $DESKTOP_PREFIX/lib)
find_library(SDL REQUIRED)
find_library(SDL_image REQUIRED)

find_library(jpeg REQUIRED)
find_library(ft2build REQUIRED)
find_library(turbojpeg REQUIRED)

pkg_check_modules(pkgs REQUIRED
  dali-core
  # sdl from emscripten
  sdl
  egl
  )

include_directories(/usr/local/include/SDL/)
set(LIBS ${LIBS} -lSDL -lSDL_image -lEGL -lGLESv2)

endif( ${EMSCRIPTEN} )

# non pkg config
include_directories(${DESKTOP_PREFIX}/include/)

add_definitions( ${CPP_DEFINES} ) # see configure_file() for *.in to *.h style

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS}")

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} ${pkgs_LDFLAGS} ${LIBS}) # "-lm"
