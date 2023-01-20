# Developer documentation for MRVN-Radiant

#### Index:
1. [Getting the source](#getting-the-source)
2. [Setting up your environment](#setting-up-your-environment)
3. [Building][#building]

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

<!-->
Win32 (using MSYS2)(recommended)
==================

Environment:
- MSYS2 (http://www.msys2.org/)
- from msys2_shell:
  pacman -S --needed base-devel
    32 bit:
  pacman -S --needed mingw-w64-i686-{toolchain,gtk2,gtkglext}
    64 bit:
  pacman -S mingw-w64-x86_64-{toolchain,gtk2,gtkglext}
    these are only strictly required for gamepacks:
  pacman -S --needed unzip svn git

build:
- Start respective mingw shell
- Switch to the directory with NetRadiant-custom source
- Typical release build:
  `make MAKEFILE_CONF=msys2-Makefile.conf BUILD=release -j$(nproc)`
- Typical developer's build:
  `make MAKEFILE_CONF=msys2-Makefile.conf DEPENDENCIES_CHECK=off INSTALL_DLLS=no BUILD=debug RADIANT_ABOUTMSG="MRVN-radiant dev build" -j$(nproc)`
- run:
- in the "install" directory, double click radiant.exe
<!-->

### 2. Windows (using Msys/MinGW)

Setup environment:
- Install [msys2](https://www.msys2.org/) and follow installation steps on their website
- Open the MingW shell
- Run:
  1. `pacman -S  {make,diffutils,gcc,binutils,pkg-config,unzip,git-core,subversion}`
  2. `pacman -S mingw-w64-x86_64-{openjpeg,gtk2,glib2,mesa,libxml2,libpng,gtkglext,zlib}`

<!-->
OSX(using X-windows)(UNTESTED)
====================

environment:
- OS X 10.5 or 10.6
- Xcode developer tools installed (OSX DVD)
- X11 (from the OS X DVD)
- MacPorts or fink or homebrew installed
- the following packages installed:
Macports (reported as working in 10.15.3):
  gtkglext pkgconfig glib2-devel libxml2 gtk2 pango atk gettext wget
Fink:
  gtkglext1 pkgconfig glib2-dev libxml2 gtk+2 gtk+2-dev pango1-xft2-ft219-dev atk1 gettext-dev wget
homebrew (does not work due to X11->Quartz switch, requires patched gtkglext):
  gtk+ gtkglext

build:
run 'make'

homebrew build:
run 'export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:/opt/X11/lib/pkgconfig
make'

run:
Switch into the install folder, and run NetRadiant.app
<!-->

## Building
When building for the first time it is recommended to run `make DEPENDENCIES_CHECK=on INSTALL_DLLS=yes` as this will check for all dependencies and install required libraries on windows.

#### Makefile arguments:
A list of arguments you can supply when running `make`
- `DEPENDENCIES_CHECK=on/off/verbose` - Enables the dependency check
- `INSTALL_DLLS=yes/no` - Downloads dlls on windows after build
- `BUILD=debug/release` - Build type
- `RADIANT_ABOUTMSG="MRVN-Radiant dev build"` - About message