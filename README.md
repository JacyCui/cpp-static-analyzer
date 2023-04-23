# A Simple CPP Static Analyzer

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
brew install cmake ninja llvm
```

Then, check your installed version of cmake, ninja, llvm, and clang by

```shell
cmake --version
ninja --version
llvm-config --version
clang --version
```

### On Ubuntu 22.04

Using [llvm apt source](https://apt.llvm.org/), run

```shell
sudo apt update 
sudo apt install lsb-release wget software-properties-common gnupg zlib1g zlib1g-dev git cmake ninja-build build-essential
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

And then, you should see something like below, which means the 255 
assertions are passed.

```shell
# a lot of log information here ...
===============================================================================
[doctest] test cases:  14 |  14 passed | 0 failed | 0 skipped
[doctest] assertions: 255 | 255 passed | 0 failed |
[doctest] Status: SUCCESS!
```

