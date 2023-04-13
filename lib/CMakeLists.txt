add_library(libanalyzer
        World.cpp
        ir/IR.cpp
        language/CPPMethod.cpp
        language/Type.cpp
        util/Logger.cpp
        )

target_link_libraries(libanalyzer
        ${LLVM_LIBS}
        ${CLANG_LIBS}
        )
