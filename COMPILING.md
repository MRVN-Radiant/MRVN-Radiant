# Compiling

## Windows

### Visual Studio ( msvc ) Recommended
- Git clone the repository
- Download the latest release from [MRVN-WinDeps](https://github.com/MRVN-Radiant/MRVN-WinDeps)
- `windeps.7z` goes into the root of radiant, so that the `windeps` folder is in the same place as the root `CMakeLists.txt`
- `install.7z` and `install-debug.7z` go into the `install` folder
- Open the repository in visual studio
- You'll probably need to generate the cmake cache, to do this open the root `CMakeLists.txt` and click **Generate** on the popup
- To build go `Build -> Build All`

### Native tools comamnd prompt for vs ( msvc )
- Follow the **Visual studio** instructions
- Open the command prompt and `cd` into the root dir of radiant
- Run `cmake . -G "Ninja"`
- Run `cmake --build .` to build

## MingW ( gcc )
### Getting dependencies
- Install [msys2](https://www.msys2.org/) and follow installation steps on their website
- Open the MingW shell
- Run:
  ```sh
  pacman -S {make,cmake,gcc,pkg-config,unzip,base-devel}
  pacman -S mingw-w64-x86_64-{cmake,openjpeg,qt5-base,glib2,libxml2,libpng,zlib}
  ```
- Use `mingw-w64-x86_64` for 64-bit and `mingw-w64-i686` for 32-bit

### Compiling
- `cd` into the root directory of MRVN-Radiant and run these commands:
  ```sh
  cmake . -G "MinGW Makefiles" -DINSTALL_DLLS=OFF
  cmake --build .
  ```
### Note
- To be able to run radiant from file explorer you'll need to copy over some dlls. Thankfully there's a script for this.
  ```sh
  cmake . -G "MinGW Makefiles" -DINSTALL_DLLS=ON
  ```
- You only need to run this once.

- If you get a white screen after launching radiant you'll need to manually delete `OPENGL32.dll` from the `install/` folder. Alternatively you can delete mesa.

## Linux
### Getting dependencies
- Dependencies:
    - qt5
    - glib
    - libxml2
    - zlib
    - libpng
    - libjpeg

### Compiling
- `cd` into the root directory of MRVN-Radiant and run these commands:
  ```sh
  cmake . -G "Unix Makefiles"
  cmake --build .
  ```

### Some more useful flags when generating a makefile
 - Example: `cmake . -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DBUILD_PLUGINS=OFF`
 - `CMAKE_VERBOSE_MAKEFILE` ON/OFF
 - `CMAKE_BUILD_TYPE` Release/Debug
 - `BUILD_PLUGINS` ON/OFF
 - `BUILD_RADIANT` ON/OFF
 - `BUILD_TOOLS` ON/OFF