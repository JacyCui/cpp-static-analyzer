# A Simple CPP Static Analysis Framework

> Copyright (C) 2023 Jiacai Cui <201220014@smail.nju.edu.cn>

## Introduction

This is a simple cpp static analysis framework developed during
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
sudo apt install lsb-release wget software-properties-common gnupg zlib1g zlib1g-dev git cmake ninja-build build-essential doxygen graphviz
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

And then, you should see something like below, which means the **597** 
testing assertions are passed.

```shell
# a lot of log information here ...
===============================================================================
[doctest] test cases:  32 |  32 passed | 0 failed | 0 skipped
[doctest] assertions: 597 | 597 passed | 0 failed |
[doctest] Status: SUCCESS!
```

### Run the Example Dataflow Analysis

After compiling, **in the project root directory**, run

```shell
./build/tools/reaching-definition-analyzer --source-dir=resources/dataflow/ReachDef
```

This will run the reaching definition analysis 
for all source files in the `resources/dataflow/ReachDef` directory.

```shell
./build/tools/reaching-definition-analyzer --help
A Simple CPP Reaching Definition Static Analyzer
Copyright (c) 2023-2023
Usage: ./build/tools/reaching-definition-analyzer/reaching-definition-analyzer [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -S,--source-dir TEXT REQUIRED
                              directory of all source files
  -I,--include-dir TEXT       directory of all header files
  --std,--standard TEXT       c++ language standard (support all standards that clang supports)
```

Similarly, **in the project root directory**, run

```shell
./build/tools/live-variable-analyzer --source-dir=resources/dataflow/LiveVar
```

This will run the live variable analysis
for all source files in the `resources/dataflow/LiveVar` directory.

```shell
./build/tools/live-variable-analyzer --help
A Simple CPP Live Variable Static Analyzer
Copyright (c) 2023-2023
Usage: ./build/tools/live-variable-analyzer [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -S,--source-dir TEXT REQUIRED
                              directory of all source files
  -I,--include-dir TEXT       directory of all header files
  --std,--standard TEXT       c++ language standard (support all standards that clang supports)
```

## How to use it as a library in your project

### Linking Steps

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


### Get the API Documentation

You can build the html doxygen api documentation locally
**in the build directory** by running

```shell
# in the build directory
ninja libanalyzer-api-doc
```

And you'll find your html documentation located at `build/docs/api-doc/html` directory.

You can read it by opening `build/docs/api-doc/html/index.html` in your local web browser.

Note that the documentation is not included in the default build target. 

You must build it explicitly like above if you need to read it.

