include(CheckIncludeFileCXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(tct_coreapi_utc)

INCLUDE(FindPkgConfig)
SET(BIN_DIR "/opt/usr/bin")

INCLUDE_DIRECTORIES(
	src/common
)

ADD_SUBDIRECTORY(src)
