cmake_minimum_required(VERSION 2.6)

if( ${EMSCRIPTEN} )
  message("EMSCRIPTEN BUILD")

  set(CMAKE_C_COMPILER "emcc")
  set(CMAKE_CXX_COMPILER "em++")
else( ${EMSCRIPTEN} )
  message("NATIVE BUILD")
#  set(CMAKE_C_COMPILER "clang")
#  set(CMAKE_CXX_COMPILER "clang++")


endif( ${EMSCRIPTEN} )

project(dali-emscripten CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(SRCS
#  ${CMAKE_SOURCE_DIR}/../../adaptors/tizen/internal/common/gl/egl-factory.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/common/gl/egl-image-extensions.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/common/gl/gl-extensions.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/emscripten/egl-implementation.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/emscripten/sdl-application.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/emscripten/sdl-gl-sync-abstraction.cpp
  ${CMAKE_SOURCE_DIR}/../../adaptors/emscripten/main.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/sdl-platform-abstraction.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/emscripten-callbacks.cpp
  ${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/dali-wrapper.cpp
)

#if( ${EMSCRIPTEN} )
#  set(SRCS ${SRCS} dali-wrapper.cpp)
#endif( ${EMSCRIPTEN} )

include_directories(${CMAKE_SOURCE_DIR}/../../)
include_directories(${CMAKE_SOURCE_DIR}/../../adaptors/)
include_directories(${CMAKE_SOURCE_DIR}/../../adaptors/tizen)
include_directories(${CMAKE_SOURCE_DIR}/../../adaptors/emscripten)
include_directories(${CMAKE_SOURCE_DIR}/../../platform-abstractions/slp)

configure_file(${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/dali-wrapper.js dali-wrapper.js COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/test-devel.js test-devel.js COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/../../platform-abstractions/emscripten/test-devel.html test-devel.html COPYONLY)

set(VENDOR "samsung")
set(PACKAGE ${PROJECT_NAME})
set(DESKTOP_PREFIX $ENV{DESKTOP_PREFIX})
set(CPP_DEFINES -DDALI_GLES_VERSION=2)

set(DEBUG_FLAGS "-Wall -g -O1")
set(RELEASE_FLAGS "-Wall -g -O1")

if( ${EMSCRIPTEN} )
 set(DEBUG_FLAGS "${DEBUG_FLAGS} -std=c++11")
 set(RELEASE_FLAGS "${RELEASE_FLAGS} -std=c++11")
endif( ${EMSCRIPTEN} )


# gles2
#find_package(OpenGLES2 REQUIRED HINTS $DESKTOP_PREFIX/include/GLES2)
#include_directories(${OpenGLES2_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${OpenGLES2_LIBRARIES})

#find_library(OpenGLES2 HINTS ${DESKTOP_PREFIX}/lib)
#find_library(EGL HINTS ${DESKTOP_PREFIX}/lib)
#find_library(SDL HINTS ${DESKTOP_PREFIX}/lib)

#define SDL_MAJOR_VERSION	1
#define SDL_MINOR_VERSION	2

#include_directories( ${OPENGLES2_INCLUDE_DIRS} )
#OPENGLES2_LIBRARIES

include(FindPkgConfig)

pkg_check_modules(pkgs REQUIRED
  #dali
  dali-core
  # sdl from emscripten
  )

if( ${EMSCRIPTEN} )

pkg_check_modules(pkgs REQUIRED
  #dali
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
  #dali
  dali-core
  # sdl from emscripten
  sdl
  egl
  )

include_directories(/usr/local/include/SDL/)
set(LIBS ${LIBS} -lSDL -lSDL_image -lEGL -lGLESv2)

endif( ${EMSCRIPTEN} )

#include_directories(/home/likewise-open/SERILOCAL/lee.morgan/installs/emscripten/emscripten/system/include)#
#
#include_directories(/home/likewise-open/SERILOCAL/lee.morgan/installs/SDL2/include)
#link_directories(/home/likewise-open/SERILOCAL/lee.morgan/installs/SDL2/build/.libs)
#set(LIBS ${LIBS} -lSDL)
#
#include_directories(/home/likewise-open/SERILOCAL/lee.morgan/installs/SDL_image-1.2.12)
#link_directories(/home/likewise-open/SERILOCAL/lee.morgan/installs/SDL_image-1.2.12/.libs)
#set(LIBS ${LIBS} -lSDL_image)

# non pkg config
include_directories(${DESKTOP_PREFIX}/include/)

find_library(gles2 REQUIRED HINTS $DESKTOP_PREFIX/lib)
find_library(egl REQUIRED HINTS $DESKTOP_PREFIX/lib)
#find_library(sdl VERSION "1.2" REQUIRED HINTS $DESKTOP_PREFIX/lib)

#find_package(SDL REQUIRED)

#  GLES2
#  EGL
#  SDL

#find_library(SDL REQUIRED)
#find_library(SDL_image REQUIRED)

add_definitions( ${CPP_DEFINES} ) # see configure_file() for *.in to *.h style

#message(STATUS "aa" ${SDL_INCLUDE_DIRS} "ffffffffffffffffffffF" )

#include_directories( "/usr/include/SDL" )
#include_directories( "/usr/local/include/SDL" )

#set(LIBS ${LIBS} ${GLES2_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${EGL_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${SDL_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASE_FLAGS}")

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} ${pkgs_LDFLAGS} "-lm" ${LIBS})

#
#ADD_SUBDIRECTORY(src)
