add_library(libanalyzer
        World.cpp
        ir/IR.cpp
        ir/CPPMethod.cpp ir/CPPMethod.h)

target_link_libraries(libanalyzer
        ${LLVM_LIBS}
        ${CLANG_LIBS}
        )
