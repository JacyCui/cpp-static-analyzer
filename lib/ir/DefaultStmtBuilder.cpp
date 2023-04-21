#include "ir/Stmt.h"

namespace analyzer::ir {

    std::shared_ptr<Stmt> DefaultStmtBuilder::buildStmt(const lang::CPPMethod &method, const clang::Stmt *clangStmt)
    {
        return std::make_shared<ClangStmtWrapper>(method, clangStmt);
    }

    std::shared_ptr<Stmt> DefaultStmtBuilder::buildEmptyStmt(const lang::CPPMethod &method)
    {
        return std::make_shared<NopStmt>(method);
    }

};
