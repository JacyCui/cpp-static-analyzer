#ifndef STATIC_ANALYZER_CPPMETHOD_H
#define STATIC_ANALYZER_CPPMETHOD_H

#include <string>
#include <memory>

#include <clang/Frontend/ASTUnit.h>

#include "language/Type.h"

namespace analyzer::language {

    /**
     * @class CPPMethod
     * @brief a class representing a cpp method
     */
    class CPPMethod final {
    public:

        /**
         * @return the string representation of the method signature (without parameter name)
         */
        [[nodiscard]] const std::string& getMethodSignatureAsString() const;

        /**
         * @return the source code of the method (declaration + definition)
         */
        [[nodiscard]] const std::string getMethodSourceCode() const;

        /**
         * @return the number of parameters of this method
         */
        [[nodiscard]] int getParamCount() const;

        /**
         * get the type of the ith parameter
         * @param i the index integer
         * @return type of the parameter
         */
        [[nodiscard]] Type getParamType(int i) const;

        /**
         * get the name of the ith parameter
         * @param i the index integer
         * @return name of the parameter
         */
        [[nodiscard]] std::string getParamName(int i) const;

        /**
         * @return the return type of the method
         */
        [[nodiscard]] Type getReturnType() const;

        // Functions below should not be called from clients

        CPPMethod(const std::unique_ptr<clang::ASTUnit>& astUnit,
                  const clang::FunctionDecl* funcDecl, const std::string& signatureStr);

        [[nodiscard]] const std::unique_ptr<clang::ASTUnit>& getASTUnit() const;

        [[nodiscard]] const clang::FunctionDecl* getFunctionDecl() const;

        CPPMethod(const CPPMethod&) = delete;

        CPPMethod& operator=(const CPPMethod&) = delete;

    private:

        const std::unique_ptr<clang::ASTUnit>& astUnit; ///< a clang ast unit

        const clang::FunctionDecl* funcDecl; ///< a function declaration, not implicit and has body

        const std::string signatureStr; ///< a signature string

    };

} // language

#endif //STATIC_ANALYZER_CPPMETHOD_H
