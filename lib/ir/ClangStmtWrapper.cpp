#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/ParentMapContext.h>

#include "language/CPPMethod.h"
#include "ir/Stmt.h"
#include "World.h"

namespace analyzer::ir {

    ClangStmtWrapper::ClangStmtWrapper(const lang::CPPMethod& method,
        const clang::Stmt* clangStmt, std::unordered_map<const clang::VarDecl*, std::shared_ptr<Var>>& varPool)
        : method(method), clangStmt(clangStmt)
    {

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
                if (vars.find(D) == vars.end()) {
                    vars.emplace(D, World::get().getVarBuilder()->buildVar(method, D));
                }
                if (D->hasInit()) {
                    defs.emplace(vars.at(D));
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
                if (parentStmt) {
                    const auto *implicitCast = clang::dyn_cast<clang::ImplicitCastExpr>(parentStmt);
                    if (implicitCast) {
                        if (implicitCast->getCastKind() == clang::CK_LValueToRValue) {
                            if (uses.find(var) == uses.end()) {
                                uses.emplace(var);
                            }
                            return true;
                        }
                    }
                }
                if (defs.find(var) == defs.end()) {
                    defs.emplace(var);
                }
                return true;
            }

            bool VisitUnaryOperator(clang::UnaryOperator* S)
            {
                if (S->isIncrementDecrementOp()) {
                    if (auto* declRef = clang::dyn_cast<clang::DeclRefExpr>(S->getSubExpr())) {
                        if (auto* varDecl = clang::dyn_cast<clang::VarDecl>(declRef->getDecl())) {
                            if (vars.find(varDecl) == vars.end()) {
                                vars.emplace(varDecl, World::get().getVarBuilder()->buildVar(method, varDecl));
                            }
                            uses.emplace(vars.at(varDecl));
                        }
                    }
                }
                return true;
            }

            bool VisitCompoundAssignOperator(clang::CompoundAssignOperator *S)
            {
                if (auto* declRef = clang::dyn_cast<clang::DeclRefExpr>(S->getLHS())) {
                    if (auto* varDecl = clang::dyn_cast<clang::VarDecl>(declRef->getDecl())) {
                        if (vars.find(varDecl) == vars.end()) {
                            vars.emplace(varDecl, World::get().getVarBuilder()->buildVar(method, varDecl));
                        }
                        uses.emplace(vars.at(varDecl));
                    }
                }
                return true;
            }

        };

        StmtProcessor stmtProcessor(varPool, uses, defs, method);
        stmtProcessor.TraverseStmt(const_cast<clang::Stmt*>(clangStmt));

        const clang::SourceManager& sourceManager = method.getASTUnit()->getSourceManager();

        startLine = static_cast<int>(sourceManager.getPresumedLineNumber(
                sourceManager.getExpansionLoc(clangStmt->getBeginLoc())));
        endLine = static_cast<int>(sourceManager.getPresumedLineNumber(
                sourceManager.getExpansionLoc(clangStmt->getEndLoc())));
        startColumn = static_cast<int>(sourceManager.getPresumedColumnNumber(
                sourceManager.getExpansionLoc(clangStmt->getBeginLoc())));
        endColumn = static_cast<int>(sourceManager.getPresumedColumnNumber(
                sourceManager.getExpansionLoc(clangStmt->getEndLoc())));

        clang::SourceRange&& range = clangStmt->getSourceRange();
        clang::CharSourceRange&& expansionRange = sourceManager.getExpansionRange(range);
        const clang::LangOptions& langOptions = method.getASTUnit()->getASTContext().getLangOpts();
        source = clang::Lexer::getSourceText(expansionRange,sourceManager, langOptions).str();
    }

    int ClangStmtWrapper::getStartLine() const
    {
        return startLine;
    }

    int ClangStmtWrapper::getEndLine() const
    {
        return endLine;
    }

    int ClangStmtWrapper::getStartColumn() const
    {
        return startColumn;
    }

    int ClangStmtWrapper::getEndColumn() const
    {
        return endColumn;
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

    std::string ClangStmtWrapper::str() const
    {
        return source;
    }

    const clang::Stmt* ClangStmtWrapper::getClangStmt() const
    {
        return clangStmt;
    }

}
