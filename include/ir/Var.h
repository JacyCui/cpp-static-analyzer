#ifndef STATIC_ANALYZER_VAR_H
#define STATIC_ANALYZER_VAR_H

#include <clang/AST/Decl.h>

#include "language/Type.h"

namespace analyzer::language {
    class CPPMethod;
}

namespace lang = analyzer::language;

namespace analyzer::ir {

    /**
     * @class Var
     * @brief Representation of method/constructor parameters and local variables.
     */
    class Var {
    public:

        /**
         * @return the method that defines this variable
         */
        [[nodiscard]] virtual const lang::CPPMethod& getMethod() const = 0;

        /**
         * @return the name of this variable
         */
        [[nodiscard]] virtual const std::string& getName() const = 0;

        /**
         * @return the type of this variable
         */
        [[nodiscard]] virtual std::shared_ptr<lang::Type> getType() const = 0;

        /**
         * @brief if two variable has the same identity, they refer to the same variable
         * @return the identity of this variable
         */
        [[nodiscard]] virtual std::uint64_t getIdentity() const = 0;
    };

    /**
     * @class VarBuilder
     * @brief the interface for variable builder
     */
    class VarBuilder {
    public:

        /**
         * @brief build a var in method defined by varDecl
         * @param method the method containing the var
         * @param varDecl clang variable declaration
         * @return the variable used in this project
         */
        [[nodiscard]] virtual std::shared_ptr<Var>
                buildVar(const lang::CPPMethod& method, const clang::VarDecl* varDecl) = 0;

    };

    /**
     * @class ClangVarWrapper
     * @brief a implementation of Var by wrapping the clang VarDecl*
     */
    class ClangVarWrapper final: public Var {
    public:

        [[nodiscard]] const lang::CPPMethod& getMethod() const override;

        [[nodiscard]] const std::string& getName() const override;

        [[nodiscard]] std::shared_ptr<lang::Type> getType() const override;

        [[nodiscard]] std::uint64_t getIdentity() const override;

        // the method below should not be called from user

        /**
         * Construct a clang wrapper
         * @param method the method that defines this variable
         * @param varDecl the clang VarDecl of this variable
         */
        ClangVarWrapper(const lang::CPPMethod& method, const clang::VarDecl* varDecl);

    private:

        const lang::CPPMethod& method; ///< the method that defines this variable

        const clang::VarDecl* varDecl; ///< the clang VarDecl of this variable

        std::string name; ///< the name of this variable

        std::shared_ptr<lang::Type> type; ///< the type of this variable

    };

    /**
     * @class DefaultVarBuilder
     * @brief the default implementation of var builder
     */
    class DefaultVarBuilder: public VarBuilder {
    public:

        std::shared_ptr<Var> buildVar(const lang::CPPMethod& method, const clang::VarDecl* varDecl) override;

    };

}

#endif //STATIC_ANALYZER_VAR_H
