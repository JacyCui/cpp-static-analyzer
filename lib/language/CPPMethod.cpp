#include <clang/Lex/Lexer.h>

#include "language/CPPMethod.h"

namespace analyzer::language {

    CPPMethod::CPPMethod(const std::unique_ptr<clang::ASTUnit> &astUnit,
                         const clang::FunctionDecl *funcDecl, const std::string& signatureStr)
        :astUnit(astUnit), funcDecl(funcDecl), signatureStr(signatureStr)
    {

    }

    const std::string CPPMethod::getMethodSourceCode() const
    {
        clang::SourceRange&& range = funcDecl->getSourceRange();
        const clang::SourceManager& sourceManager = astUnit->getSourceManager();
        clang::CharSourceRange&& expansionRange = sourceManager.getExpansionRange(range);
        const clang::LangOptions& langOptions = astUnit->getASTContext().getLangOpts();
        llvm::StringRef sourceCode = clang::Lexer::getSourceText(
                expansionRange,sourceManager, langOptions);
        return sourceCode.str();
    }

} // language
