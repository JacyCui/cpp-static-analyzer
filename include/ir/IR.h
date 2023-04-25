#ifndef STATIC_ANALYZER_IR_H
#define STATIC_ANALYZER_IR_H

#include <memory>
#include <clang/AST/Decl.h>

#include "ir/Stmt.h"
#include "analysis/graph/CFG.h"

namespace analyzer::language {
    class CPPMethod;
}

namespace analyzer::ir {

    namespace lang = language;
    namespace graph = analysis::graph;

    /**
     * @class IR
     * @brief interface for intermediate representation
     */
    class IR {
    public:

        /**
         * @return the cpp method this ir is representing
         */
        [[nodiscard]] virtual const lang::CPPMethod& getMethod() const = 0;

        /**
         * @return the cfg corresponding to this ir
         */
        [[nodiscard]] virtual std::shared_ptr<graph::CFG> getCFG() const = 0;

        /**
         * @return the variables corresponding to parameters
         */
        [[nodiscard]] virtual std::vector<std::shared_ptr<Var>> getParams() const = 0;

        /**
         * @return the variables in this ir
         */
        [[nodiscard]] virtual std::vector<std::shared_ptr<Var>> getVars() const = 0;

        /**
         * @return the statements in this ir
         */
        [[nodiscard]] virtual std::vector<std::shared_ptr<Stmt>> getStmts() const = 0;

        virtual ~IR() = default;

    };

    /**
     * @class DefaultIR
     * @brief the default implementation of ir
     */
    class DefaultIR final: public IR {
    public:

        [[nodiscard]] const lang::CPPMethod& getMethod() const override;

        [[nodiscard]] std::shared_ptr<graph::CFG> getCFG() const override;

        [[nodiscard]] std::vector<std::shared_ptr<Var>> getParams() const override;

        [[nodiscard]] std::vector<std::shared_ptr<Var>> getVars() const override;

        [[nodiscard]] std::vector<std::shared_ptr<Stmt>> getStmts() const override;

        // functions below should not be called from user

        /**
         * @brief Construct a default ir of method
         * @param method the method this ir is representing
         * @param params the parameter variables in this ir
         * @param vars the variables concerned in this ir
         * @param stmts the statements of this ir
         * @param cfg the cfg derived from this ir
         */
        DefaultIR(const lang::CPPMethod& method,
                  std::vector<std::shared_ptr<Var>> params,
                  std::vector<std::shared_ptr<Var>> vars,
                  std::vector<std::shared_ptr<Stmt>> stmts,
                  const std::shared_ptr<graph::CFG>& cfg);

    private:

        const lang::CPPMethod& method; ///< the method this ir is representing

        std::vector<std::shared_ptr<Var>> params; ///< the parameter variables in this ir

        std::vector<std::shared_ptr<Var>> vars; ///< the variables concerned in this ir

        std::vector<std::shared_ptr<Stmt>> stmts; ///< the statements of this ir

        std::shared_ptr<graph::CFG> cfg; ///< the cfg derived from this ir

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

        virtual ~IRBuilder() = default;

    };

    /**
     * @class DefaultIRBuilder
     * @brief build a default IR for a cpp method with the help of clang
     */
    class DefaultIRBuilder: public IRBuilder {
    public:

        // the method below should not be called from user

        [[nodiscard]] std::shared_ptr<IR> buildIR(const lang::CPPMethod& method) const override;

    };

    /**
     * @class DefaultIRBuilderHelper
     * @brief a helper class for default ir builder
     */
    class DefaultIRBuilderHelper {
    public:

        // the method below should not be called from user

        /**
         * @brief construct a helper to build the default ir of method
         * @param method a cpp method
         */
        explicit DefaultIRBuilderHelper(const lang::CPPMethod& method);

        /**
         * @return a default ir built from method
         */
        std::shared_ptr<IR> build();

    private:

        const lang::CPPMethod& method; ///< the method to build ir

        std::vector<std::shared_ptr<Var>> params; ///< the parameter variables

        std::unordered_map<const clang::VarDecl*, std::shared_ptr<Var>> varPool; ///< all variables concerned

        std::unordered_map<const clang::Stmt*, std::shared_ptr<Stmt>> stmts; ///< all non-empty statements in this ir

        std::vector<std::shared_ptr<Stmt>> stmtVec; ///< all statements in this ir

        /**
         * @brief build parameter variables
         */
        void buildParams();

        /**
         * @brief build statements
         */
        void buildStmts();

        /**
         * @brief build cfg edges
         * @param cfg the default cfg to manipulate
         */
        void buildEdges(std::shared_ptr<graph::DefaultCFG>& cfg);

    };

} // ir

#endif //STATIC_ANALYZER_IR_H
