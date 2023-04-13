#include <clang/Lex/Lexer.h>

#include "language/CPPMethod.h"

namespace analyzer::language {

    CPPMethod::CPPMethod(const std::unique_ptr<clang::ASTUnit> &astUnit,
                         const clang::FunctionDecl *funcDecl, const std::string& signatureStr)
        :astUnit(astUnit), funcDecl(funcDecl), signatureStr(signatureStr)
    {

    }

    const std::unique_ptr<clang::ASTUnit>& CPPMethod::getASTUnit() const
    {
        return astUnit;
    }


    const clang::FunctionDecl* CPPMethod::getFunctionDecl() const
    {
        return funcDecl;
    }

    const std::string& CPPMethod::getMethodSignatureAsString() const
    {
        return signatureStr;
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

    int CPPMethod::getParamCount() const
    {
        return funcDecl->getNumParams();
    }

    Type CPPMethod::getParamType(int i) const
    {
        return Type(funcDecl->getParamDecl(i)->getType());
    }

    std::string CPPMethod::getParamName(int i) const
    {
        return funcDecl->getParamDecl(i)->getNameAsString();
    }

    Type CPPMethod::getReturnType() const
    {
        return Type(funcDecl->getReturnType());
    }


} // language
