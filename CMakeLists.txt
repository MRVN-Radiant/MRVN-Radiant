cmake_minimum_required(VERSION 3.25)


# Project
project(mrvnradiant)


# Options
option(VERBOSE "Verbose output" OFF)
option(BUILD "Built type" Release) # Release / Debug
option(BUILD_PLUGINS "Build plugins" ON)
option(BUILD_RADIANT "Build radiant" ON)
option(BUILD_TOOLS   "Build tools" ON)
option(INSTALL_DLLS  "Install DLLs" OFF)

# Why are you trying to do this on linux
if(NOT WIN32)
    set(INSTALL_DLLS OFF)
endif()

# Install dlls is separate from the build process
if(INSTALL_DLLS)
    set(BUILD_PLUGINS OFF)
    set(BUILD_RADIANT OFF)
    set(BUILD_TOOLS   OFF)
endif()

set(CMAKE_VERBOSE_MAKEFILE ${VERBOSE})
set(CMAKE_BUILD_TYPE ${BUILD})

# Github actions moment
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Build system setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/install)


# Radiant definitions
add_definitions(-DRADIANT_VERSION="0.0")
add_definitions(-DRADIANT_MAJOR_VERSION=0)
add_definitions(-DRADIANT_MINOR_VERSION=0)
add_definitions(-DRADIANT_ABOUTMSG="Dev build")

# Remap definitions
add_definitions(-DQ3MAP_VERSION="Dev build")

if(WIN32)
    add_definitions(-DRADIANT_EXECUTABLE="exe")
    add_compile_definitions(WIN32)
    add_compile_definitions(_WIN32)
else()
    add_definitions(-DRADIANT_EXECUTABLE="")
    add_compile_definitions(POSIX)
endif()

if(BUILD EQUAL Debug)
    add_compile_definitions(_DEBUG)
else()
    add_compile_definitions(NDEBUG)
endif()

# Compile defs
add_compile_definitions(QT_NO_KEYWORDS)


# Find dependencies
include(FindPkgConfig)
pkg_check_modules(GLIB glib-2.0 REQUIRED)
pkg_check_modules(PNG libpng REQUIRED)
pkg_check_modules(LIBXML2 libxml-2.0 REQUIRED)
pkg_check_modules(ZLIB zlib REQUIRED)

include(FindJPEG)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# Global include dirs
include_directories(include libs)


# Libs
if(BUILD_RADIANT OR BUILD_TOOLS OR BUILD_PLUGINS)
    add_subdirectory(libs)
endif()

# Contrib
if(BUILD_PLUGINS)
    add_subdirectory(contrib)
endif()

# Plugins && Radiant
if(BUILD_RADIANT)
    add_subdirectory(plugins)
    add_subdirectory(radiant)
endif()

# Tools
if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()

# https://stackoverflow.com/questions/11944060/how-to-detect-target-architecture-using-cmake
if(INSTALL_DLLS)
    execute_process( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
    execute_process( COMMAND sh ./install-dlls-msys2-mingw.sh ${ARCHITECTURE})
endif()