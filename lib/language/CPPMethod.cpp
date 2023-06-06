#include <clang/Lex/Lexer.h>

#include <utility>

#include "World.h"
#include "language/CPPMethod.h"

namespace analyzer::language {

    CPPMethod::CPPMethod(const std::unique_ptr<clang::ASTUnit> &astUnit,
                         const clang::FunctionDecl *funcDecl, std::string  signatureStr)
        :astUnit(astUnit), funcDecl(funcDecl), signatureStr(std::move(signatureStr)), myIR(nullptr)
    {
        paramCount = funcDecl->getNumParams();
        for (unsigned int i = 0; i < paramCount; i++) {
            paramTypes.emplace_back(
                    World::get().getTypeBuilder()->buildType(funcDecl->getParamDecl(i)->getType())
                    );
            paramNames.emplace_back(funcDecl->getParamDecl(i)->getNameAsString());
        }
        returnType = World::get().getTypeBuilder()->buildType(funcDecl->getReturnType());
        clangCFG = clang::CFG::buildCFG(funcDecl, funcDecl->getBody(),
                                        &funcDecl->getASTContext(),
                                        clang::CFG::BuildOptions());
    }

    const std::unique_ptr<clang::ASTUnit>& CPPMethod::getASTUnit() const
    {
        return astUnit;
    }


    const clang::FunctionDecl* CPPMethod::getFunctionDecl() const
    {
        return funcDecl;
    }

    const std::unique_ptr<clang::CFG>& CPPMethod::getClangCFG() const
    {
        return clangCFG;
    }

    const std::string& CPPMethod::getMethodSignatureAsString() const
    {
        return signatureStr;
    }

    std::string CPPMethod::getMethodSourceCode() const
    {
        clang::SourceRange&& range = funcDecl->getSourceRange();
        const clang::SourceManager& sourceManager = astUnit->getSourceManager();
        clang::CharSourceRange&& expansionRange = sourceManager.getExpansionRange(range);
        const clang::LangOptions& langOptions = astUnit->getASTContext().getLangOpts();
        llvm::StringRef sourceCode = clang::Lexer::getSourceText(
                expansionRange,sourceManager, langOptions);
        return sourceCode.str();
    }

    std::string CPPMethod::getContainingFilePath() const
    {
        return astUnit->getSourceManager().getFilename(funcDecl->getLocation()).str();
    }

    std::size_t CPPMethod::getParamCount() const
    {
        return paramCount;
    }

    const std::vector<std::shared_ptr<Type>>& CPPMethod::getParamTypes() const
    {
        return paramTypes;
    }

    std::shared_ptr<Type> CPPMethod::getParamType(std::size_t i) const
    {
        return paramTypes.at(i);
    }

    const std::string& CPPMethod::getParamName(std::size_t i) const
    {
        return paramNames.at(i);
    }

    std::shared_ptr<Type> CPPMethod::getReturnType() const
    {
        return returnType;
    }

    std::shared_ptr<ir::IR> CPPMethod::getIR()
    {
        if (!myIR) {
            myIR = World::get().getIRBuilder()->buildIR(*this);
        }
        return myIR;
    }

    bool CPPMethod::isGlobalMethod() const
    {
        return funcDecl->isGlobal();
    }

    bool CPPMethod::isClassStaticMethod() const
    {
        return funcDecl->isStatic();
    }

    bool CPPMethod::isClassMemberMethod() const
    {
        return funcDecl->isCXXClassMember();
    }

    bool CPPMethod::isVirtual() const
    {
        if(const auto* cxxMethodDecl = clang::dyn_cast<clang::CXXMethodDecl>(funcDecl)) {
            return cxxMethodDecl->isVirtual();
        }
        return false;
    }

} // language
