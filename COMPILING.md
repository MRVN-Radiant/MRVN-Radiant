# Developer documentation for MRVN-Radiant

#### Index:
1. [Getting the source](#getting-the-source)
2. [Setting up your environment](#setting-up-your-environment)
3. [Building](#building)

## Getting the source
The latest source is available from the git repository:
  - https://github.com/MRVN-Radiant/MRVN-Radiant

## Setting up your environment

### 1. Linux(using X-windows)
Environment:
- gcc >= version 3.1 (preferably)
- GNU make
- svn >= 1.1 (some build steps use svn)

Dependencies:
- gtk+ >= 2.4.0 (requires glib, atk, pango, iconv, etc)
- gtkglext >= 1.0.0 (requires opengl)
- libxml2 >= 2.0.0
- zlib >= 1.2.0 (for archivezip module)
- libpng >= 1.2.0 (for imagepng module)

### 2. Windows (using Msys/MinGW)

Setup environment:
- Install [msys2](https://www.msys2.org/) and follow installation steps on their website
- Open the MingW shell
- Run:
  1. `pacman -S  {make,diffutils,gcc,binutils,pkg-config,unzip,git-core,subversion}`
  2. `pacman -S mingw-w64-x86_64-{openjpeg,gtk2,glib2,libxml2,libpng,gtkglext,zlib}`

## Building
When building for the first time it is recommended to run `make DEPENDENCIES_CHECK=on INSTALL_DLLS=yes` as this will check for all dependencies and install required libraries on windows.

#### Makefile arguments:
A list of arguments you can supply when running `make`
- `DEPENDENCIES_CHECK=on/off/verbose` - Enables the dependency check
- `INSTALL_DLLS=yes/no` - Downloads dlls on windows after build
- `BUILD=debug/release` - Build type
- `RADIANT_ABOUTMSG="MRVN-Radiant dev build"` - About message