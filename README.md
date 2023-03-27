# MRVNRadiant

The open-source, cross-platform level editor for Respawn Entertainment Source based games.

MRVNRadiant is a fork of NetRadiant-custom (GtkRadiant 1.4 &rarr; massive rewrite &rarr; 1.5 &rarr; NetRadiant &rarr; NetRadiant-custom &rarr; this)

<div align=left>
<img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/MRVNRadiant/MRVNRadiant/build.yml?style=for-the-badge">
<img alt="GitHub Issues" src="https://img.shields.io/github/issues/MRVNRadiant/MRVNRadiant?style=for-the-badge">
<img alt="GitHub Pullrequests" src="https://img.shields.io/github/issues-pr/MRVNRadiant/MRVNRadiant?style=for-the-badge">
</div>

## Supported games
- Titanfall Online ( Titanfall Online Revival )
- Titanfall 2 ( [Northstar](https://northstar.tf) )
- Apex Legends ( [R5Reloaded](https://r5reloaded.com/) )

## Status
| Game | Coverage | Note |
|------|----------|------|
| Titanfall Online | 38% | Basic collision, no lighting, needs testing |
| Titanfall 2 | 37% | Basic collision, no lighting, meshes flicker |
| Apex Legends | 25% | No collision, no lighting |

> Coverage = Lumps generated / Total lump count. Stubbed lumps arent counted because they're not custom.

> NOTE: These values are updated manually.

## Compiling
### Getting dependencies on Windows (Using MinGW)
- Install [msys2](https://www.msys2.org/) and follow installation steps on their website
- Open the MingW shell
- Run:
  ```sh
  pacman -S {make,cmake,gcc,pkg-config,unzip,base-devel}
  pacman -S mingw-w64-x86_64-{cmake,openjpeg,qt5-base,glib2,libxml2,libpng,zlib}
  ```
- Use `mingw-w64-x86_64` for 64-bit and `mingw-w64-i686` for 32-bit

### Compiling on Windows
`cd` into the root directory of MRVN-Radiant and run these commands:
```sh
cmake . -G "MinGW Makefiles" - DINSTALL_DLLS=OFF
cmake --build .
```
### Note
To be able to run radiant from file explorer you'll need to copy over some dlls. Thankfully there's a script for this.
```sh
cmake . -G "MinGW Makefiles" -DINSTALL_DLLS=ON
```
You only need to run this once.

If you get a white screen after launching radiant you'll need to manually delete `OPENGL32.dll` from the `install/` folder. Alternatively you can delete mesa.

### Getting dependencies on Linux
- Dependencies:
    - qt5
    - glib
    - libxml2
    - zlib
    - libpng

### Compiling on Linux
`cd` into the root directory of MRVN-Radiant and run these commands:
```sh
cmake . -G "Unix Makefiles"
cmake --build .
```

## Other tools
- [bsp_tool](https://github.com/snake-biscuits/bsp_tool)
    - A Python library for analyzing .bsp files

- [MRVN-Explorer](https://github.com/MRVN-Radiant/MRVN-Explorer)
    - A OpenGL/ImGui respawn bsp viewer
