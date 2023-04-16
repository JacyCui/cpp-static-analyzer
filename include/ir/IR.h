#ifndef STATIC_ANALYZER_IR_H
#define STATIC_ANALYZER_IR_H

#include <memory>

#include <clang/Analysis/CFG.h>

#include "Stmt.h"

namespace analyzer::language {
    class CPPMethod;
}

namespace lang = analyzer::language;

namespace analyzer::ir {

    class IR {
    public:

        [[nodiscard]] virtual const lang::CPPMethod& getMethod() const = 0;



    };

    class DefaultIR final: public IR {
    public:

        [[nodiscard]] const lang::CPPMethod& getMethod() const override;

        DefaultIR(const lang::CPPMethod& method, std::shared_ptr<Var> thisVar,
                  std::vector<std::shared_ptr<Var>> params,
                  std::vector<std::shared_ptr<Var>> vars,
                  std::vector<std::shared_ptr<Var>> returnVars,
                  std::vector<std::shared_ptr<Stmt>> stmts);

    private:

        const lang::CPPMethod& method; ///< the method this ir is representing

        std::shared_ptr<Var> thisVar;

        const std::vector<std::shared_ptr<Var>> params;

        const std::vector<std::shared_ptr<Var>> vars;

        const std::vector<std::shared_ptr<Var>> returnVars;

        const std::vector<std::shared_ptr<Stmt>> stmts;

    };

    /**
     * @class IRBuilder
     * @brief Interface for builder of IR
     */
    class IRBuilder {
    public:
        /**
         * @brief Builds IR for concrete methods.
         * @param method the method used to build ir
         * @return the intermediate representation of method
         */
        [[nodiscard]] virtual std::shared_ptr<IR> buildIR(const lang::CPPMethod& method) const = 0;
    };

    /**
     * @class DefaultIRBuilder
     * @brief build a default IR for a cpp method with the help of clang
     */
    class DefaultIRBuilder: public IRBuilder {
    public:
        [[nodiscard]] std::shared_ptr<IR> buildIR(const lang::CPPMethod& method) const override;
    };

    class DefaultIRBuilderHelper {
    public:
        explicit DefaultIRBuilderHelper(const lang::CPPMethod& method);

        std::shared_ptr<IR> build();

    private:
        const lang::CPPMethod& method;
        std::shared_ptr<Var> thisVar;
        std::vector<std::shared_ptr<Var>> params;
        std::vector<std::shared_ptr<Var>> vars;
        std::vector<std::shared_ptr<Var>> returnVars;
        std::vector<std::shared_ptr<Stmt>> stmts;
    };

} // ir

#endif //STATIC_ANALYZER_IR_H
