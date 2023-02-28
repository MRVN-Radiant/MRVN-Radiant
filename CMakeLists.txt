cmake_minimum_required(VERSION 3.10)

#
project(mrvnradiant)

# 
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED True)


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