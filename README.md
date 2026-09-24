
# NoRo. Framework
[![Test](https://github.com/SunnyMatta/NoRo/actions/workflows/NoRoTests.yml/badge.svg?branch=main)](https://github.com/SunnyMatta/NoRo/actions/workflows/NoRoTests.yml)

NoRo (Row Notation) Is a Lightweight, High-Performance Graphical Framework Written in C. Built on top of GLFW.

![damaged_helmet example](images/1.png)
<div align="center">
<sub>NoRo Demo | Damaged Helmet by Daly Realism & Oscar Cafaro. KhronosGroup</sub>
</div>

#
![sponza example](images/2.png)
<div align="center">
<sub>NoRo Demo | Sponza by Marko Dabrovic at RNA Studio (original) & Frank Meinl at Crytek (Super Sponza)</sub>
</div>

## Core Features

- Texture Streaming: 
    - The Framework uses `libktx` pipeline, which allows to load KTX2 supercompressed textures (Basis Universal/UASTC) directly into GPU VRAM, minimizing runtime transcoding overhead.
- glTF 2.0 Asset load:
    - Full PBR material mapping and mesh rendering using `cgltf` for file parsing

## Demo
[Demo video (youtube)](https://youtu.be/GX43e0w-IgY)

## Dependencies
### Windows
You can have your own method to load dependencies, but it is highly recommended to download dependencies from vcpkg:

`vcpkg install ktx:x64-windows glfw3:x64-windows glew:x64-windows`

After that, all dependencies will be downloaded at your vcpkg root directory (By default, vcpkg root directory got env: `$VCPKG_ROOT`)

CMake Compiling requires you to set toolchain file, luckly it's easy to set up
When compiling, add `-DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows` to your command.

>[!IMPORTANT]
>If you'll get issues with vcpkg when compiling, consider turning off manifest mode by using `-DVCPKG_MANIFEST_MODE=OFF`

>[!NOTE]
> It is considered bad practice to set toolchains inside `CMakeLists.txt`. If you know what you are doing, setting toolchain in the file requires put `set(CMAKE_TOOLCHAIN_FILE "$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "Toolchain file")` **BEFORE the project() call**

### Linux

Install KTX2 Library from [Khronos's Github Repository](https://github.com/KhronosGroup/KTX-Software/releases)
Then install other dependencies for your distro:
**For Fedora:**

    sudo dnf group install c-development
    sudo dnf install cmake glfw-devel glew-devel

**For Debian-based:**

    sudo apt update && sudo apt install build-essential cmake git libglfw3-dev libglew-dev

**For Arch-based:**

    sudo pacman -Syu base-devel glfw glew

## Getting Started
In order to get started, please check `Example/main.c`

### Demo

Execute `demo.sh/bat` for simple demo (WASD-controls for flying, Q/E for up/down)

## Third-Party Licenses
**Thanks** to everyone who made/maintaining these repositories!

`cglm` by [recp](https://github.com/recp) (https://github.com/recp/cglm) "MIT LICENSE"

`cgltf` by [jkuhlmann](https://github.com/jkuhlmann) (https://github.com/jkuhlmann/cgltf) "MIT LICENSE"

`stb` by [nothings](https://github.com/nothings) (https://github.com/nothings/stb/) "MIT LICENSE & Public Domain"
