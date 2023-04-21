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
sudo apt install llvm
sudo apt install clang
sudo apt install ninja-build
```



## Compile Project and Run Tests to Check Setup



