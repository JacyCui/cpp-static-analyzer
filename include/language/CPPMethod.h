#ifndef STATIC_ANALYZER_CPPMETHOD_H
#define STATIC_ANALYZER_CPPMETHOD_H

#include <string>
#include <memory>

#include <clang/Frontend/ASTUnit.h>
#include <clang/Analysis/CFG.h>

#include "language/Type.h"
#include "ir/IR.h"

namespace analyzer::language {

    /**
     * @class CPPMethod
     * @brief Represents methods in the program. Each instance contains various
     * information of a method, including method name, signature, method body (IR), etc.
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
        [[nodiscard]] std::string getMethodSourceCode() const;

        /**
         * @return the relative path of the file containing this method
         */
        [[nodiscard]] std::string getContainingFilePath() const;

        /**
         * @return the number of parameters of this method
         */
        [[nodiscard]] std::size_t getParamCount() const;

        /**
         * @return the list of parameter types of this method
         */
        [[nodiscard]] const std::vector<std::shared_ptr<Type>>& getParamTypes() const;

        /**
         * @brief get the type of the ith parameter
         * @param i the index integer
         * @return type of the parameter
         */
        [[nodiscard]] std::shared_ptr<Type> getParamType(std::size_t i) const;

        /**
         * @brief get the name of the ith parameter
         * @param i the index integer
         * @return name of the parameter
         */
        [[nodiscard]] const std::string& getParamName(std::size_t i) const;

        /**
         * @return the return type of the method
         */
        [[nodiscard]] std::shared_ptr<Type> getReturnType() const;

        /**
         * @return the intermediate representation of this method body
         */
        [[nodiscard]] std::shared_ptr<ir::IR> getIR();

        /**
         * @return whether the method is a global method
         */
        [[nodiscard]] bool isGlobalMethod() const;

        /**
         * @return whether the method is a class static method
         */
        [[nodiscard]] bool isClassStaticMethod() const;

        /**
         * @return whether the method is a class member method
         */
        [[nodiscard]] bool isClassMemberMethod() const;

        /**
         * @return whether the method is virtual
         */
        [[nodiscard]] bool isVirtual() const;

        // Functions below should not be called from clients

        /**
         * Construct a cpp method definition
         * @param astUnit the ast unit of the cpp file
         * @param funcDecl function declaration ast node
         * @param signatureStr method signature
         */
        CPPMethod(const std::unique_ptr<clang::ASTUnit>& astUnit,
                  const clang::FunctionDecl* funcDecl, std::string signatureStr);

        /**
         * @return the ast unit of the cpp file containing this method
         */
        [[nodiscard]] const std::unique_ptr<clang::ASTUnit>& getASTUnit() const;

        /**
         * @return function declaration ast node of this method
         */
        [[nodiscard]] const clang::FunctionDecl* getFunctionDecl() const;

        /**
         * @return the clang cfg of this method
         */
        [[nodiscard]] const std::unique_ptr<clang::CFG>& getClangCFG() const;

        CPPMethod(const CPPMethod&) = delete;

        CPPMethod& operator=(const CPPMethod&) = delete;

    private:

        std::size_t paramCount; ///< the number of parameters

        std::vector<std::shared_ptr<Type>> paramTypes; ///< all parameter types

        std::vector<std::string> paramNames; ///< all parameter names

        std::shared_ptr<Type> returnType; ///< the return type

        std::shared_ptr<ir::IR> myIR; ///< intermediate representation of the function body

        const std::unique_ptr<clang::ASTUnit>& astUnit; ///< a clang ast unit

        const clang::FunctionDecl* funcDecl; ///< a function declaration, not implicit and has body

        const std::string signatureStr; ///< a signature string

        std::unique_ptr<clang::CFG> clangCFG; ///< the clang cfg of this method
    };

} // language

#endif //STATIC_ANALYZER_CPPMETHOD_H
