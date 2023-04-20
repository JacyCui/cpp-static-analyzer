add_library(libanalyzer
        World.cpp
        ir/DefaultIR.cpp
        ir/DefaultIRBuilder.cpp
        ir/ClangVarWrapper.cpp
        ir/ClangStmtWrapper.cpp
        language/CPPMethod.cpp
        language/Type.cpp
        language/DefaultTypeBuilder.cpp
        util/Logger.cpp
        analysis/graph/DefaultCFG.cpp
        ir/DefaultStmtBuilder.cpp ir/DefaultVarBuilder.cpp ir/NopStmt.cpp)

target_link_libraries(libanalyzer
        ${LLVM_LIBS}
        ${CLANG_LIBS}
        )
