# atlas

[![Build status](https://ci.appveyor.com/api/projects/status/9fh5xc7vxhvx8ba5?svg=true)](https://ci.appveyor.com/project/sguimmara/atlas)

## What is atlas ?
*atlas* is a cross platform (Windows and Linux) experimental globe renderer written against the Vulkan graphics API.

## Features
*atlas* is not designed with a set of features in mind, but rather as an exploration of the Vulkan API
for massively parallel planet rendering.

## Build instructions

The definitive guide to building atlas is located in the [appveyor](appveyor.yml) configuration file, for both Linux and Windows. By reproducing the steps located in the `install` and `build_script` sections for your configuration (Linux or Windows), you should be able to build atlas effortlessly.

### Preliminary
Install the dependencies :
* [GLFW](https://www.glfw.org/download.html) 3.2.1 or above : Provides windowing and peripheral support.
* The [Vulkan SDK](https://vulkan.lunarg.com/) 1.1.92 or above : Provides the vulkan graphics API.
* [CMake](https://cmake.org/download/) 3.7.0 or above is required for proper Vulkan support.
* A [compiler](https://en.cppreference.com/w/cpp/compiler_support#cpp17) with good C++ 17 support : clang 6.0, MSVC 15, or GCC 8.

Clone the repository, then initialize the submodules with

    git submodule update --init

### Linux and Windows
Create a build directory in the working tree :

    mkdir .build

Initialize CMake in this directory:

    cd .build
    cmake ..

Then build the project:

    cmake --build .

The compiled binaries are located in the `.build/bin` folder.

Note : On Linux, compiling errors occur may indicate that the compiler
does not support C++17. This can happen if there is an older
GCC version installed and used by default. In this case, set the
`CC` and `CXX` environment variables to point to the compatible C and C++
compiler executables, respectively. For example, if you wish to use Clang
instead of GCC :

    export CC=/usr/bin/clang
    export CXX=/usr/bin/clang++
    cmake ..
    cmake --build .
