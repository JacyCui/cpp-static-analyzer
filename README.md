# A Simple CPP Static Analysis Framework

> Copyright (C) 2022 Jiacai Cui <201220014@smail.nju.edu.cn>

## Introduction

This is a simple cpp static analyzer developed during
a software engineering lab course of Nanjing University.

The project is personal and distributed
under [GNU general public license](LICENSE).

The design of this project is well inspired by 
[tai-e](https://github.com/pascal-lab/Tai-e.git), 
a static analysis framework for java, please view 
its [technique report](https://arxiv.org/abs/2208.00337) 
for more details.

## Environment Setup

This cpp static analyzer uses clang as its front end. 
So you need to install llvm and clang on your local 
system inorder to build and run this project. 
It is developed under [llvm](https://llvm.org/) 
16.0.2, tested under llvm 16.0.2 and 17.0.0, 
but more recent versions should also be ok.

It is recommended to install llvm using precompiled binaries
instead of building from source manually. 

Here is the way to set up the proper environment of this
project.

### On macOS

Use [homebrew](https://brew.sh/) as the package manager, run 

```shell
brew install cmake ninja llvm doxygen
```

Then, check your installed version of cmake, ninja, llvm, and clang by

```shell
cmake --version
ninja --version
llvm-config --version
clang --version
doxygen --version
```

### On Ubuntu 22.04

Using [llvm apt source](https://apt.llvm.org/), run

```shell
sudo apt update 
sudo apt install lsb-release wget software-properties-common gnupg zlib1g zlib1g-dev git cmake ninja-build build-essential doxygen
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
./llvm.sh 17 all
```

Then, check your installed version of cmake, ninja, llvm, and clang by

```shell
cmake --version
ninja --version
clang-17 --version
llvm-config-17 --version
doxygen --version
```

## Get the Project

```shell
git clone https://github.com/JacyCui/cpp-static-analyzer.git
```

### Build

In the project root directory, run

```shell
mkdir build
cd build
cmake -G=Ninja ..
ninja
```

### Run Tests

After compiling, **in the project root directory**, run

```shell
./build/tests/tests
```

And then, you should see something like below, which means the **411** 
assertions are passed.

```shell
# a lot of log information here ...
===============================================================================
[doctest] test cases:  19 |  19 passed | 0 failed | 0 skipped
[doctest] assertions: 411 | 411 passed | 0 failed |
[doctest] Status: SUCCESS!
```

## Get the API Documentation

An online doxygen api documentation can be found [here](https://jacycui.github.io/cpp-static-analyzer/).
You can also build a local version of it **in the build directory**.
Run

```shell
# in the build directory
ninja libanalyzer-api-doc
```

And you'll find your html documentation in the `build/docs/api-doc/html` directory.

## How to use it as a library in your project

**Step01**: Take this repository as a submodule of your project repository.

```shell
git submodule add https://github.com/JacyCui/cpp-static-analyzer.git path/to/put/this/project
```

**Step02**: Link to `libanalyzer` in you `CMakeLists.txt`.

```cmake
# suppose your target is called your_target

add_subdirectory(path/to/put/this/project)

target_include_directories(your_target
        PUBLIC path/to/put/this/project/include
        )

target_link_libraries(your_target
        libanalyzer
        )
```

**Step03**: Use APIs provided [here](https://jacycui.github.io/cpp-static-analyzer/) in your source code.
An example usage is provided in the [test of reaching definition](tests/TestReachingDefinition.cpp).



