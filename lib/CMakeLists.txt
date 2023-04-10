add_library(libanalyzer
        World.cpp
        )

target_link_libraries(libanalyzer
        ${LLVM_LIBS}
        ${CLANG_LIBS}
        )
