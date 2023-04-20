#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/ParentMapContext.h>

#include "language/CPPMethod.h"
#include "ir/Stmt.h"
#include "World.h"

namespace analyzer::ir {

    ClangStmtWrapper::ClangStmtWrapper(const lang::CPPMethod& method, const clang::Stmt* clangStmt)
        : method(method), clangStmt(clangStmt)
    {

        std::unordered_map<const clang::VarDecl*, std::shared_ptr<Var>> vars;

        class StmtProcessor: public clang::RecursiveASTVisitor<StmtProcessor> {
        private:

            std::unordered_map<const clang::VarDecl*, std::shared_ptr<Var>>& vars;

            std::unordered_set<std::shared_ptr<Var>>& uses;

            std::unordered_set<std::shared_ptr<Var>>& defs;

            const lang::CPPMethod& method;

        public:

            StmtProcessor(std::unordered_map<const clang::VarDecl*, std::shared_ptr<Var>>& vars,
                          std::unordered_set<std::shared_ptr<Var>> &uses,
                          std::unordered_set<std::shared_ptr<Var>> &defs,
                          const lang::CPPMethod& method)
                :vars(vars), uses(uses), defs(defs), method(method)
            {

            }

            bool VisitVarDecl(clang::VarDecl* D)
            {
                std::shared_ptr<Var> var = World::get().getVarBuilder()->buildVar(method, D);
                vars.emplace(D, var);
                if (D->hasInit()) {
                    defs.emplace(var);
                }
                return true;
            }

            bool VisitDeclRefExpr(clang::DeclRefExpr* S)
            {
                auto* varDecl = clang::dyn_cast<clang::VarDecl>(S->getDecl());
                if (!varDecl) {
                    return true;
                }
                if (vars.find(varDecl) == vars.end()) {
                    vars.emplace(varDecl, World::get().getVarBuilder()->buildVar(method, varDecl));
                }
                std::shared_ptr<Var> var = vars.at(varDecl);
                clang::DynTypedNodeList parents = method.getASTUnit()->getASTContext().getParents(*S);
                if (parents.empty()) {
                    if (defs.find(var) == defs.end()) {
                        defs.emplace(var);
                    }
                    return true;
                }
                const auto* parentStmt = parents[0].get<clang::Stmt>();
                const auto* implicitCast = clang::dyn_cast<clang::ImplicitCastExpr>(parentStmt);
                if (implicitCast) {
                    if (implicitCast->getCastKind() == clang::CK_LValueToRValue) {
                        if (uses.find(var) == uses.end()) {
                            uses.emplace(var);
                        }
                        return true;
                    }
                }
                if (defs.find(var) == defs.end()) {
                    defs.emplace(var);
                }
                return true;
            }

        };

        StmtProcessor stmtProcessor(vars, uses, defs, method);
        stmtProcessor.TraverseStmt(const_cast<clang::Stmt*>(clangStmt));

        const clang::SourceManager& sourceManager = method.getASTUnit()->getSourceManager();

    }

    int ClangStmtWrapper::getStartLine() const
    {
        return -1;
    }

    int ClangStmtWrapper::getEndLine() const
    {
        return -1;
    }

    int ClangStmtWrapper::getStartColumn() const
    {
        return -1;
    }

    int ClangStmtWrapper::getEndColumn() const
    {
        return -1;
    }

    const lang::CPPMethod& ClangStmtWrapper::getMethod() const
    {
        return method;
    }

    std::unordered_set<std::shared_ptr<Var>> ClangStmtWrapper::getDefs() const
    {
        return defs;
    }

    std::unordered_set<std::shared_ptr<Var>> ClangStmtWrapper::getUses() const
    {
        return uses;
    }

}
