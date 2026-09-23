
# NoRo. Framework
NoRo (Row Notation) Is a Lightweight, High-Performance Graphical Framework Written in C with memory management. Built on top of GLfw.

![damaged_helmet example](images/1.png)
![sponza example](images/2.png)
## Core Features

- Texture Streaming: 
    - Integrated `libktx` pipeline to load KTX2 supercompressed textures (Basis Universal/UASTC) directly into GPU VRAM, minimizing runtime transcoding overhead.
- glTF 2.0 Asset load:
    - Full PBR material mapping and mesh rendering using `cgltf` for parsing

## Demo
[Demo video (youtube)](https://youtu.be/GX43e0w-IgY)

## Dependencies

Firstly, install KTX2 Library from [Khronos's Github Repository](https://github.com/KhronosGroup/KTX-Software/releases)

**For Fedora:**

    sudo dnf install glfw-devel glew-devel

**For Debian-based:**

    sudo apt update && sudo apt install libglfw3-dev libglew-dev

**For Arch-based:**

    sudo pacman -S glfw glew

## Getting Started
In order to get started, please check `Example/main.c`

### Demo

Execute `run.sh/bat` for demo

## Third-Party Licenses
**Thanks** to everyone who made/maintaining these repositories!

`cglm` by [recp](https://github.com/recp) (https://github.com/recp/cglm) "MIT LICENSE"

`cgltf` by [jkuhlmann](https://github.com/jkuhlmann) (https://github.com/jkuhlmann/cgltf) "MIT LICENSE"

`stb` by [nothings](https://github.com/nothings) (https://github.com/nothings/stb/) "MIT LICENSE & Public Domain"
