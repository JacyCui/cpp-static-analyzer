#include "ir/Stmt.h"

namespace analyzer::ir {

    std::shared_ptr<Stmt> DefaultStmtBuilder::buildStmt(const lang::CPPMethod &method,
        const clang::Stmt *clangStmt, std::unordered_map<const clang::VarDecl*, std::shared_ptr<Var>>& varPool)
    {
        return std::make_shared<ClangStmtWrapper>(method, clangStmt, varPool);
    }

    std::shared_ptr<Stmt> DefaultStmtBuilder::buildEmptyStmt(const lang::CPPMethod &method)
    {
        return std::make_shared<NopStmt>(method);
    }

};
