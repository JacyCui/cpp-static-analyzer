#ifndef STATIC_ANALYZER_SOLVER_H
#define STATIC_ANALYZER_SOLVER_H

#include "analysis/dataflow/fact/DataflowResult.h"
#include "analysis/dataflow/analysis/DataflowAnalysis.h"

#include <memory>

namespace analyzer::analysis::dataflow::solver {

    /**
     * @class Solver
     * @brief Interface of data-flow analysis solver.
     * @tparam Fact
     */
    template <typename Fact>
    class Solver {
    public:

        /**
         * @brief solves the given analysis problem
         * @param dataFlowAnalysis a dataflow analysis problem
         * @return the analysis result
         */
        [[nodiscard]] virtual std::shared_ptr<fact::DataflowResult<Fact>>
            solve(const std::unique_ptr<analysis::DataFlowAnalysis<Fact>>& dataFlowAnalysis) const = 0;

    };

    /**
     * @class AbstractSolver
     * @brief provides common functionalities for Solver
     * @tparam Fact type of data-flow facts
     */
    template <typename Fact>
    class AbstractSolver: public Solver<Fact> {
    public:

        [[nodiscard]] std::shared_ptr<fact::DataflowResult<Fact>>
            solve(const std::unique_ptr<analysis::DataFlowAnalysis<Fact>>& dataFlowAnalysis) const override
        {

        }

    protected:

        virtual void initializeForward(
                const std::unique_ptr<analysis::DataFlowAnalysis<Fact>>& dataFlowAnalysis,
                std::shared_ptr<fact::DataflowResult<Fact>> result) const
        {
            std::shared_ptr<graph::CFG> cfg = dataFlowAnalysis->getCFG();
            std::shared_ptr<ir::Stmt> entry = cfg->getEntry();
            result->setInFact(entry, dataFlowAnalysis->newBoundaryFact());
            result->setOutFact(entry, dataFlowAnalysis->newBoundaryFact());
            for (const std::shared_ptr<ir::Stmt>& stmt : cfg->getIR()->getStmts()) {
                result->setInFact(stmt, dataFlowAnalysis->newInitialFact());
                result->setOutFact(stmt, dataFlowAnalysis->newInitialFact());
            }
            std::shared_ptr<ir::Stmt> exit = cfg->getExit();
            result->setInFact(exit, dataFlowAnalysis->newInitialFact());
            result->setOutFact(exit, dataFlowAnalysis->newInitialFact());
        }

        virtual void initializeBackward(
                const std::unique_ptr<analysis::DataFlowAnalysis<Fact>>& dataFlowAnalysis,
                std::shared_ptr<fact::DataflowResult<Fact>> result) const
        {
            std::shared_ptr<graph::CFG> cfg = dataFlowAnalysis->getCFG();
            std::shared_ptr<ir::Stmt> exit = cfg->getExit();
            result->setInFact(exit, dataFlowAnalysis->newBoundaryFact());
            result->setOutFact(exit, dataFlowAnalysis->newBoundaryFact());
            for (const std::shared_ptr<ir::Stmt>& stmt : cfg->getIR()->getStmts()) {
                result->setInFact(stmt, dataFlowAnalysis->newInitialFact());
                result->setOutFact(stmt, dataFlowAnalysis->newInitialFact());
            }
            std::shared_ptr<ir::Stmt> entry = cfg->getEntry();
            result->setInFact(entry, dataFlowAnalysis->newInitialFact());
            result->setOutFact(entry, dataFlowAnalysis->newInitialFact());
        }

        virtual void doSolveForward(
                const std::unique_ptr<analysis::DataFlowAnalysis<Fact>>& dataFlowAnalysis,
                std::shared_ptr<fact::DataflowResult<Fact>> result) const = 0;

        virtual void doSolveBackward(
                const std::unique_ptr<analysis::DataFlowAnalysis<Fact>>& dataFlowAnalysis,
                std::shared_ptr<fact::DataflowResult<Fact>> result) const = 0;

    private:

        [[nodiscard]] std::shared_ptr<fact::DataflowResult<Fact>>
            initialize(const std::unique_ptr<analysis::DataFlowAnalysis<Fact>>& dataFlowAnalysis) const
        {
            std::shared_ptr<graph::CFG> cfg = dataFlowAnalysis->getCFG();
            std::shared_ptr<fact::DataflowResult<Fact>> result =
                    std::make_shared<fact::DataflowResult<Fact>>();
            if (dataFlowAnalysis->isForward()) {
                initializeForward(dataFlowAnalysis, result);
            } else {
                initializeBackward(dataFlowAnalysis, result);
            }
            return result;
        }

        void doSolve(const std::unique_ptr<analysis::DataFlowAnalysis<Fact>>& dataFlowAnalysis,
                     std::shared_ptr<fact::DataflowResult<Fact>> result) const
        {
            if (dataFlowAnalysis->isForward()) {
                doSolveForward(dataFlowAnalysis, result);
            } else {
                doSolveBackward(dataFlowAnalysis, result);
            }
        }

    };

    template <typename Fact>
    class WorkListSolver: public AbstractSolver<Fact> {

    };

    /**
     * @tparam Fact the dataflow fact
     * @brief factory method for obtaining a solver of a given dataflow fact
     * @return a solver implemented by worklist
     */
    template <typename Fact>
    std::unique_ptr<Solver<Fact>> makeSolver() {
        return std::make_unique<WorkListSolver<Fact>>();
    }

} // solver

#endif //STATIC_ANALYZER_SOLVER_H
