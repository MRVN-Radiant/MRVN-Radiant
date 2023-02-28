cmake_minimum_required(VERSION 3.12)

#
project(mrvnradiant)

# 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Definitions
add_definitions(-DRADIANT_VERSION="0.0")
add_definitions(-DRADIANT_MAJOR_VERSION=0)
add_definitions(-DRADIANT_MINOR_VERSION=0)
add_definitions(-DRADIANT_ABOUTMSG="dev build")

if(WIN32)
    add_definitions(-DRADIANT_EXECUTABLE="exe")
else()
    add_definitions(-DRADIANT_EXECUTABLE="")
endif()

add_compile_definitions(POSIX)

# Find dependencies
include(FindPkgConfig)
pkg_check_modules(GLIB glib-2.0 REQUIRED)
pkg_check_modules(XML libxml-2.0 REQUIRED)
pkg_check_modules(PNG libpng REQUIRED)

# Find QT
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# Global include dirs
include_directories(include libs)


# Radiant
add_subdirectory(radiant)