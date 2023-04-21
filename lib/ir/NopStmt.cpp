#include "ir/Stmt.h"

namespace analyzer::ir {

    NopStmt::NopStmt(const lang::CPPMethod& method)
        :method(method)
    {

    }

    const lang::CPPMethod& NopStmt::getMethod() const
    {
        return method;
    }

    std::unordered_set<std::shared_ptr<Var>> NopStmt::getDefs() const
    {
        return {};
    }

    std::unordered_set<std::shared_ptr<Var>> NopStmt::getUses() const
    {
        return {};
    }

    int NopStmt::getStartLine() const
    {
        return -1;
    }

    int NopStmt::getEndLine() const
    {
        return -1;
    }

    int NopStmt::getStartColumn() const
    {
        return -1;
    }

    int NopStmt::getEndColumn() const
    {
        return -1;
    }

    std::string NopStmt::str() const
    {
        return "nop";
    }

    const clang::Stmt* NopStmt::getClangStmt() const
    {
        return nullptr;
    }

}
