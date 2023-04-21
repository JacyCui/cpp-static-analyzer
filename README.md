# A Simple CPP Static Analyzer

## Environment Setup

This app static analyzer uses clang as its front end. 
So you need to install llvm and clang on your system. 
This is developed and tested under [llvm](https://llvm.org/) 
16.0.2 (which is also the latest version until 2023.4.21), 
but recent versions should be ok.

It is recommended to install llvm using a package manager
instead of building from source manually. 

Here is the way to install the latest version of llvm, clang 
and ninja.

### On MacOS

Use [homebrew](https://brew.sh/) as the package manager, run 

```shell
brew install llvm
brew install ninja
```

Then, check your installed version of llvm, clang and ninja by

```shell
llvm-config --version
clang --version
ninja --version
```

### On Ubuntu

Using [apt](https://ubuntu.com/server/docs/package-management) as the package manager,
adding llvm to software source, run

```shell
sudo apt install clang-format clang-tidy clang-tools clang clangd libc++-dev libc++1 libc++abi-dev libc++abi1 libclang-dev libclang1 liblldb-dev libllvm-ocaml-dev libomp-dev libomp5 lld lldb llvm-dev llvm-runtime llvm python3-clang
sudo apt install ninja-build
```

Then, check your installed version of llvm, clang and ninja by

```shell
llvm-config --version
clang --version
ninja --version
```

## Compile Project and Run Tests to Check Setup

To compile this project, in the project root directory, run

```shell
mkdir build
cd build
cmake -G=Ninja ..
ninja
```

To run tests, after building, **in the project root directory**, run

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



