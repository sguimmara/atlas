# atlas

## What is atlas ?
*atlas* is a cross platform (Windows and Linux) experimental globe renderer written against the Vulkan graphics API.

## Features
*atlas* is not designed with a set of features in mind, but rather as an exploration of the Vulkan API
for massively parallel planet rendering.

## Build instructions

### Preliminary
Install the dependencies :
* GLFW 3.2.1 or above : used to work with windows and peripheral inputs
* The Vulkan SDK 1.1.92 or above : used to render the graphics
* CMake 3.7.0 or above
* A compiler with good C++ 17 support : clang 6.0, MSVC 15, or GCC 8

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

Note : if any compiling errors occur, it may indicate that the compiler
used is not compatible with C++ 17. This can happen if there is an older
GCC version installed and used by default. In this case, set the
`CC` and `CXX` environment variables to point to the compatible C and C++
compiler executables, respectively. For example, if you wish to use Clang
instead of GCC :

    export CC=/usr/bin/clang
    export CXX=/usr/bin/clang++
    cmake ..
    cmake --build .
