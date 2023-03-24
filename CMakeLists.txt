cmake_minimum_required(VERSION 3.25)

# Project
project(mrvnradiant)

# set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
# set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES OFF)


# Options
option(VERBOSE "Verbose output" OFF)
option(BUILD "Built type" Release)
option(INSTALL_DLLS "Install DLLs" ON)

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

set(CMAKE_NO_SYSTEM_FROM_IMPORTED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/install)

set(CMAKE_VERBOSE_MAKEFILE ${VERBOSE})

set(CMAKE_BUILD_TYPE ${BUILD})


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
else()
    add_definitions(-DRADIANT_EXECUTABLE="")
    add_compile_definitions(POSIX)
endif()

# Compile defs
add_compile_definitions(QT_NO_KEYWORDS)


# Find dependencies
include(FindPkgConfig)
pkg_check_modules(GLIB glib-2.0 REQUIRED)
pkg_check_modules(PNG libpng REQUIRED)
pkg_check_modules(LIBXML2 libxml-2.0 REQUIRED)

include(FindJPEG)
# find_package(LibXml2 REQUIRED)
#find_package(OpenGL REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# Global include dirs
include_directories(include libs)


# # Libs
add_subdirectory(libs)

# # Plugins
add_subdirectory(plugins)

# # # Radiant
add_subdirectory(radiant)

# # # Tools
add_subdirectory(tools)


# https://stackoverflow.com/questions/11944060/how-to-detect-target-architecture-using-cmake
# if(INSTALL_DLLS)
# EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
# message( STATUS "Archiaaaaatecture: ${ARCHITECTURE}" )
# execute_process( COMMAND sh ./install-dlls-msys2-mingw.sh ${ARCHITECTURE})
# endif()


# Copy gamepacks
# add_custom_target(gamepacks ALL
#                   COMMAND ${CMAKE_COMMAND} -E copy_directory
#                   ${CMAKE_SOURCE_DIR}/games ${CMAKE_SOURCE_DIR}/build/gamepacks)

# # Copy bitmaps
# add_custom_target(resources ALL
#                   COMMAND ${CMAKE_COMMAND} -E copy_directory
#                   ${CMAKE_SOURCE_DIR}/setup/data/tools/ ${CMAKE_SOURCE_DIR}/build)

# Copy bitmaps
# add_custom_target(install_dlls ALL
#                   COMMAND sh ./install-dlls-msys2-mingw.sh)