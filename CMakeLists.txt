cmake_minimum_required(VERSION 3.14)

project(cpp-static-analyzer
        VERSION 1.0.0
        DESCRIPTION "This is a simple cpp static analysis framework."
        )

set(CMAKE_CXX_STANDARD 17) # the standard used in this project and llvm
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti") # add for those build llvm without rtti

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

find_package(Clang REQUIRED CONFIG)
message(STATUS "Found Clang ${LLVM_VERSION}")
message(STATUS "Using ClangConfig.cmake in ${Clang_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(LLVM_LIBS support frontendopenmp)

# define CLANG_LIBS: some clang libs to be used in link time
# !!! DO NOT change the order of the libs !!!
# libs will be linked in this order, and changing the order could lead to link errors
set(CLANG_LIBS
        clangTooling
        clangFrontendTool
        clangFrontend
        clangDriver
        clangSerialization
        clangCodeGen
        clangParse
        clangSema
        clangStaticAnalyzerFrontend
        clangStaticAnalyzerCheckers
        clangStaticAnalyzerCore
        clangAnalysis
        clangARCMigrate
        clangRewriteFrontend
        clangEdit
        clangAST
        clangASTMatchers
        clangLex
        clangBasic
        )

include_directories(include)

add_subdirectory(lib)
add_subdirectory(tools)

enable_testing()
add_subdirectory(tests)

add_subdirectory(docs)
