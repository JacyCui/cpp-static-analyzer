add_library(libanalyzer
        World.cpp
        ir/IR.cpp
        )

target_link_libraries(libanalyzer
        ${LLVM_LIBS}
        ${CLANG_LIBS}
        )
