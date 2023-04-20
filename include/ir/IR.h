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

    class IR {
    public:

        [[nodiscard]] virtual const lang::CPPMethod& getMethod() const = 0;

        [[nodiscard]] virtual std::shared_ptr<graph::CFG> getCFG() const = 0;

        [[nodiscard]] virtual std::vector<std::shared_ptr<Var>> getParams() const = 0;

        [[nodiscard]] virtual std::vector<std::shared_ptr<Var>> getVars() const = 0;

        [[nodiscard]] virtual std::shared_ptr<Var> getVarByIdentity(std::uint64_t id) const = 0;

        [[nodiscard]] virtual std::vector<std::shared_ptr<Stmt>> getStmts() const = 0;

    };

    class DefaultIR final: public IR {
    public:

        [[nodiscard]] const lang::CPPMethod& getMethod() const override;

        [[nodiscard]] std::shared_ptr<graph::CFG> getCFG() const override;

        [[nodiscard]] virtual std::vector<std::shared_ptr<Var>> getParams() const override;

        [[nodiscard]] virtual std::vector<std::shared_ptr<Var>> getVars() const override;

        [[nodiscard]] virtual std::shared_ptr<Var> getVarByIdentity(std::uint64_t id) const override;

        [[nodiscard]] virtual std::vector<std::shared_ptr<Stmt>> getStmts() const override;

        DefaultIR(const lang::CPPMethod& method,
                  const std::vector<std::shared_ptr<Var>>& params,
                  const std::unordered_map<std::uint64_t, std::shared_ptr<Var>>& vars,
                  const std::vector<std::shared_ptr<Stmt>>& stmts,
                  const std::shared_ptr<graph::CFG>& cfg);

    private:

        const lang::CPPMethod& method; ///< the method this ir is representing

        const std::vector<std::shared_ptr<Var>> params;

        const std::unordered_map<std::uint64_t, std::shared_ptr<Var>> vars;

        const std::vector<std::shared_ptr<Stmt>> stmts;

        std::shared_ptr<graph::CFG> cfg;

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

        std::vector<std::shared_ptr<Var>> params;

        std::unordered_map<std::uint64_t, std::shared_ptr<Var>> vars;

        std::unordered_map<const clang::Stmt*, std::shared_ptr<Stmt>> stmts;

        void buildParams();

        void buildStmts();

        void buildEdges(std::shared_ptr<graph::DefaultCFG>& cfg);

    };

} // ir

#endif //STATIC_ANALYZER_IR_H
