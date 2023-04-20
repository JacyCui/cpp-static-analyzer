#include "ir/Var.h"

namespace analyzer::ir {

    std::shared_ptr<Var> DefaultVarBuilder::buildVar(const lang::CPPMethod& method, const clang::VarDecl* varDecl)
    {
        return std::make_shared<ClangVarWrapper>(method, varDecl);
    }

}
