#ifndef STATIC_ANALYZER_SOLVER_H
#define STATIC_ANALYZER_SOLVER_H

#include <memory>
#include <queue>

#include "analysis/dataflow/fact/DataflowResult.h"
#include "analysis/dataflow/DataflowAnalysis.h"

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
         * @param dataflowAnalysis a dataflow analysis problem
         * @return the analysis result
         */
        [[nodiscard]] virtual std::shared_ptr<fact::DataflowResult<Fact>>
            solve(const std::unique_ptr<DataflowAnalysis<Fact>>& dataflowAnalysis) const = 0;

        virtual ~Solver() = default;

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
            solve(const std::unique_ptr<DataflowAnalysis<Fact>>& dataflowAnalysis) const override
        {
            World::getLogger().Info("Initializing dataflow facts ...");
            std::shared_ptr<fact::DataflowResult<Fact>> result = initialize(dataflowAnalysis);
            World::getLogger().Info("Doing the solving ...");
            doSolve(dataflowAnalysis, result);
            return result;
        }

    protected:

        /**
         * @brief initialize the data facts for a forward analysis
         * @param dataflowAnalysis the corresponding dataflow analysis
         * @param result the result to be initialized
         */
        virtual void initializeForward(
                const std::unique_ptr<DataflowAnalysis<Fact>>& dataflowAnalysis,
                std::shared_ptr<fact::DataflowResult<Fact>> result) const
        {
            std::shared_ptr<graph::CFG> cfg = dataflowAnalysis->getCFG();
            std::shared_ptr<ir::Stmt> entry = cfg->getEntry();
            result->setInFact(entry, dataflowAnalysis->newBoundaryFact());
            result->setOutFact(entry, dataflowAnalysis->newBoundaryFact());
            for (const std::shared_ptr<ir::Stmt>& stmt : cfg->getIR()->getStmts()) {
                result->setInFact(stmt, dataflowAnalysis->newInitialFact());
                result->setOutFact(stmt, dataflowAnalysis->newInitialFact());
            }
            std::shared_ptr<ir::Stmt> exit = cfg->getExit();
            result->setInFact(exit, dataflowAnalysis->newInitialFact());
            result->setOutFact(exit, dataflowAnalysis->newInitialFact());
        }

        /**
         * @brief initialize the data facts for a backward analysis
         * @param dataflowAnalysis the corresponding dataflow analysis
         * @param result the result to be initialized
         */
        virtual void initializeBackward(
                const std::unique_ptr<DataflowAnalysis<Fact>>& dataflowAnalysis,
                std::shared_ptr<fact::DataflowResult<Fact>> result) const
        {
            std::shared_ptr<graph::CFG> cfg = dataflowAnalysis->getCFG();
            std::shared_ptr<ir::Stmt> exit = cfg->getExit();
            result->setInFact(exit, dataflowAnalysis->newBoundaryFact());
            result->setOutFact(exit, dataflowAnalysis->newBoundaryFact());
            for (const std::shared_ptr<ir::Stmt>& stmt : cfg->getIR()->getStmts()) {
                result->setInFact(stmt, dataflowAnalysis->newInitialFact());
                result->setOutFact(stmt, dataflowAnalysis->newInitialFact());
            }
            std::shared_ptr<ir::Stmt> entry = cfg->getEntry();
            result->setInFact(entry, dataflowAnalysis->newInitialFact());
            result->setOutFact(entry, dataflowAnalysis->newInitialFact());
        }

        /**
         * @brief solve a forward dataflow analysis
         * @param dataflowAnalysis the dataflow analysis to be solved
         * @param result where to store the results
         */
        virtual void doSolveForward(
                const std::unique_ptr<DataflowAnalysis<Fact>>& dataflowAnalysis,
                std::shared_ptr<fact::DataflowResult<Fact>> result) const = 0;

        /**
         * @brief solve a backward dataflow analysis
         * @param dataflowAnalysis the dataflow analysis to be solved
         * @param result where to store the results
         */
        virtual void doSolveBackward(
                const std::unique_ptr<DataflowAnalysis<Fact>>& dataflowAnalysis,
                std::shared_ptr<fact::DataflowResult<Fact>> result) const = 0;

    private:

        [[nodiscard]] std::shared_ptr<fact::DataflowResult<Fact>>
            initialize(const std::unique_ptr<DataflowAnalysis<Fact>>& dataflowAnalysis) const
        {
            std::shared_ptr<graph::CFG> cfg = dataflowAnalysis->getCFG();
            std::shared_ptr<fact::DataflowResult<Fact>> result =
                    std::make_shared<fact::DataflowResult<Fact>>();
            if (dataflowAnalysis->isForward()) {
                this->initializeForward(dataflowAnalysis, result);
            } else {
                this->initializeBackward(dataflowAnalysis, result);
            }
            return result;
        }

        void doSolve(const std::unique_ptr<DataflowAnalysis<Fact>>& dataflowAnalysis,
                     std::shared_ptr<fact::DataflowResult<Fact>> result) const
        {
            if (dataflowAnalysis->isForward()) {
                this->doSolveForward(dataflowAnalysis, result);
            } else {
                this->doSolveBackward(dataflowAnalysis, result);
            }
        }

    };

    /**
     * @class WorkListSolver
     * @brief a Work-list solver
     * @tparam Fact type of dataflow fact
     */
    template <typename Fact>
    class WorkListSolver: public AbstractSolver<Fact> {
    protected:

        void doSolveForward(
                const std::unique_ptr<DataflowAnalysis<Fact>>& dataflowAnalysis,
                std::shared_ptr<fact::DataflowResult<Fact>> result) const override
        {
            std::queue<std::shared_ptr<ir::Stmt>> workList;
            std::shared_ptr<graph::CFG> cfg = dataflowAnalysis->getCFG();
            workList.push(cfg->getEntry());
            for (std::shared_ptr<ir::Stmt>& s : cfg->getIR()->getStmts()) {
                workList.push(s);
            }
            workList.push(cfg->getExit());
            while (!workList.empty()) {
                std::shared_ptr<ir::Stmt> stmt = workList.front();
                workList.pop();
                if (stmt == cfg->getEntry()) {
                    continue;
                }
                for (const std::shared_ptr<ir::Stmt>& pred : cfg->getPredsOf(stmt)) {
                    dataflowAnalysis->meetInto(result->getOutFact(pred),
                                               result->getInFact(stmt));
                }
                if (dataflowAnalysis->transferNode(stmt,
                    result->getInFact(stmt), result->getOutFact(stmt))) {
                    for (const std::shared_ptr<ir::Stmt>& succ : cfg->getSuccsOf(stmt)) {
                        workList.push(succ);
                    }
                }
            }
        }

        void doSolveBackward(
                const std::unique_ptr<DataflowAnalysis<Fact>>& dataflowAnalysis,
                std::shared_ptr<fact::DataflowResult<Fact>> result) const override
        {
            std::queue<std::shared_ptr<ir::Stmt>> workList;
            std::shared_ptr<graph::CFG> cfg = dataflowAnalysis->getCFG();
            workList.push(cfg->getExit());
            for (std::shared_ptr<ir::Stmt>& s: cfg->getIR()->getStmts()) {
                workList.push(s);
            }
            workList.push(cfg->getEntry());
            while (!workList.empty()) {
                std::shared_ptr<ir::Stmt> stmt = workList.front();
                workList.pop();
                if (stmt == cfg->getExit()) {
                    continue;
                }
                for (const std::shared_ptr<ir::Stmt>& succ : cfg->getSuccsOf(stmt)) {
                    dataflowAnalysis->meetInto(result->getInFact(succ),
                                               result->getOutFact(stmt));
                }
                if (dataflowAnalysis->transferNode(stmt,
                    result->getInFact(stmt), result->getOutFact(stmt))) {
                    for (const std::shared_ptr<ir::Stmt>& pred : cfg->getPredsOf(stmt)) {
                        workList.push(pred);
                    }
                }
            }
        }

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
