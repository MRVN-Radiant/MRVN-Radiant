cmake_minimum_required(VERSION 3.12)

# Project
project(mrvnradiant)

# Options
option(VERBOSE "Verbose output" OFF)
option(BUILD "Built type" Release)


# Build system setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-fPIC")

set(CMAKE_C_FLAGS "-fPIC")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

set(CMAKE_VERBOSE_MAKEFILE ${VERBOSE})

set(CMAKE_BUILD_TYPE ${BUILD})

# Definitions
add_definitions(-DRADIANT_VERSION="0.0")
add_definitions(-DRADIANT_MAJOR_VERSION=0)
add_definitions(-DRADIANT_MINOR_VERSION=0)
add_definitions(-DRADIANT_ABOUTMSG="Dev build")

if(WIN32)
    add_definitions(-DRADIANT_EXECUTABLE="exe")
else()
    add_definitions(-DRADIANT_EXECUTABLE="")
endif()

add_compile_definitions(POSIX)

# Find dependencies
include(FindPkgConfig)
pkg_check_modules(GLIB glib-2.0 REQUIRED)
pkg_check_modules(PNG libpng REQUIRED)

find_package(LibXml2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# Global include dirs
include_directories(include libs)


# Libs
add_subdirectory(libs)

# Radiant
add_subdirectory(radiant)

# Plugins
add_subdirectory(plugins)


# Copy gamepacks
add_custom_target(gamepacks ALL
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_SOURCE_DIR}/games ${CMAKE_SOURCE_DIR}/build/gamepacks)

# Copy bitmaps
add_custom_target(resources ALL
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_SOURCE_DIR}/setup/data/tools/ ${CMAKE_SOURCE_DIR}/build)
