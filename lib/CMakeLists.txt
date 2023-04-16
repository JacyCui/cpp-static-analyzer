add_library(libanalyzer
        World.cpp
        ir/DefaultIR.cpp
        ir/DefaultIRBuilder.cpp
        ir/exp/Var.cpp
        language/CPPMethod.cpp
        language/Type.cpp
        util/Logger.cpp
        )

target_link_libraries(libanalyzer
        ${LLVM_LIBS}
        ${CLANG_LIBS}
        )
