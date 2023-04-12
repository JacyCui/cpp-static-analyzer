#ifndef STATIC_ANALYZER_CPPMETHOD_H
#define STATIC_ANALYZER_CPPMETHOD_H

#include <string>
#include <memory>

#include <clang/Frontend/ASTUnit.h>

namespace analyzer::language {

    class CPPMethod final {
    private:
        const std::unique_ptr<clang::ASTUnit>& astUnit;
        const clang::FunctionDecl* funcDecl;
        const std::string signatureStr;


    public:
        CPPMethod(const std::unique_ptr<clang::ASTUnit>& astUnit,
                  const clang::FunctionDecl* funcDecl, const std::string& signatureStr);

        CPPMethod(const CPPMethod&) = delete;
        CPPMethod& operator=(const CPPMethod&) = delete;

        [[nodiscard]] const std::unique_ptr<clang::ASTUnit>& getASTUnit() const
        {
            return astUnit;
        }

        [[nodiscard]] const clang::FunctionDecl* getFunctionDecl() const
        {
            return funcDecl;
        }

        [[nodiscard]] const std::string& getMethodSignatureAsString() const
        {
            return signatureStr;
        }

        const std::string getMethodSourceCode() const;
    };

} // language

#endif //STATIC_ANALYZER_CPPMETHOD_H
