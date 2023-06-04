cmake_minimum_required(VERSION 3.12)


# Project
project(mrvnradiant VERSION 0.0)


# Options
option(BUILD_PLUGINS "Build plugins" ON)
option(BUILD_RADIANT "Build radiant" ON)
option(BUILD_TOOLS   "Build tools" ON)
option(INSTALL_DLLS  "Install DLLs" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

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

# Github actions moment
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# Build system setup
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compile flags
set(CMAKE_CXX_FLAGS_RELEASE "-Os")
set(CMAKE_C_FLAGS_RELEASE "")

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    add_compile_definitions(RADIANT_EXECUTABLE="")
    add_compile_definitions(POSIX)
    add_compile_definitions(XWINDOWS)
endif()

if(WIN32)
    add_compile_definitions(RADIANT_EXECUTABLE=".exe")
    add_compile_definitions(WIN32)
    add_compile_definitions(_WIN32)
endif()

# Debug
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(_DEBUG)
else()
    add_compile_definitions(NDEBUG)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/install)


# Radiant definitions
add_compile_definitions(RADIANT_VERSION="0.0")
add_compile_definitions(RADIANT_MAJOR_VERSION=0)
add_compile_definitions(RADIANT_MINOR_VERSION=0)
add_compile_definitions(RADIANT_ABOUTMSG="Dev build")

# Remap definitions
add_compile_definitions(Q3MAP_VERSION="Dev build")

# Compile defs
add_compile_definitions(QT_NO_KEYWORDS)

# Find dependencies
if (${MSVC}) # Visual studio
    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/windeps/cmake)
    set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/windeps/qt)
    find_package(Glib2 REQUIRED)
    find_package(LibPng REQUIRED)
    find_package(LibXml2 REQUIRED)
    find_package(ZLib REQUIRED)
    find_package(LibJpeg REQUIRED)
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
    find_package(Threads REQUIRED)
else() # Msys / Linux
    include(FindPkgConfig)
    pkg_check_modules(GLIB glib-2.0 REQUIRED)
    pkg_check_modules(PNG libpng REQUIRED)
    pkg_check_modules(LIBXML2 libxml-2.0 REQUIRED)
    pkg_check_modules(ZLIB zlib REQUIRED)
    pkg_check_modules(JPEG libjpeg REQUIRED)

    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
    find_package(Threads REQUIRED)
endif()

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
