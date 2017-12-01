VulkanTutorial
--------------

Following [the Vulkan Tutorial](https://vulkan-tutorial.com).
[![license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/SRombauts/VulkanTutorial/blob/master/LICENSE.txt)
[![Travis CI Linux Build Status](https://travis-ci.org/SRombauts/VulkanTutorial.svg)](https://travis-ci.org/SRombauts/VulkanTutorial "Travis CI Linux Build Status")
[![AppVeyor Windows Build status](https://ci.appveyor.com/api/projects/status/github/SRombauts/VulkanTutorial?svg=true)](https://ci.appveyor.com/project/SbastienRombauts/VulkanTutorial "AppVeyor Windows Build status")

Copyright (c) 2017 Sébastien Rombauts (sebastien.rombauts@gmail.com)


## Building with CMake

find_package(Vulkan) requires CMake 3.7, but I have added the FindVulkan.cmake module in the source tree.

Dependencies:

- glfw   Windowing library is provided as a submodule
- glm    Math matrix/vector library is provided as a submodule

GLFW require the installation of the following:

 sudo apt-get install libvulkan-dev libxrandr-dev libxinerama-dev libxcursor-dev 

### Get glm and glfw submodules

```bash
git submodule init
git submodule update
```

### Typical generic build (see also "build.bat" or "./build.sh")

```bash
mkdir build
cd build
cmake ..        # cmake .. -G "Visual Studio 2017"    # for Visual Studio 2017
cmake --build . # make
```

### Debug build for Unix Makefiles

```bash
mkdir Debug
cd Debug
cmake .. -DCMAKE_BUILD_TYPE=Debug   # -G "Unix Makefiles"
cmake --build . # make
```

### Release build

```bash
mkdir Release
cd Release
cmake .. -DCMAKE_BUILD_TYPE=Release  # -G "Unix Makefiles"
cmake --build . # make
```
