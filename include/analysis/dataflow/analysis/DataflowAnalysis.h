#ifndef STATIC_ANALYZER_DATAFLOWANALYSIS_H
#define STATIC_ANALYZER_DATAFLOWANALYSIS_H

#include "World.h"
#include "analysis/graph/CFG.h"
#include "analysis/Analysis.h"
#include "analysis/dataflow/fact/DataflowResult.h"
#include "analysis/dataflow/solver/Solver.h"

namespace analyzer::analysis::dataflow::analysis {

    /**
     * @brief Template interface for defining data-flow analysis.
     * @tparam Fact type of data-flow facts
     */
    template<typename Fact>
    class DataFlowAnalysis {

        /**
         * @return true if this analysis is forward, otherwise false.
         */
        [[nodiscard]] virtual bool isForward() const = 0;

        /**
         * @return new fact in boundary conditions, i.e., the fact for
         * entry (exit) node in forward (backward) analysis.
         */
        [[nodiscard]] virtual std::shared_ptr<Fact> newBoundaryFact() const = 0;

        /**
         * @return new initial fact for non-boundary nodes.
         */
        [[nodiscard]] virtual std::shared_ptr<Fact> newInitialFact() const = 0;

        /**
         * @brief Meets a fact into another (target) fact.
         * This function will be used to handle control-flow confluences.
         * @param fact fact to be meet
         * @param target target fact
         */
        virtual void meetInto(std::shared_ptr<Fact> fact, std::shared_ptr<Fact> target) const = 0;

        /**
         * @brief Node Transfer function for the analysis.
         * The function transfers data-flow from in (out) fact to out (in) fact
         * for forward (backward) analysis.
         * @param stmt stmt to be transferred
         * @param in in facts
         * @param out out facts
         * @return true if the transfer changed the out (in) fact, otherwise false.
         */
        virtual bool transferNode(std::shared_ptr<ir::Stmt> stmt, std::shared_ptr<Fact> in, std::shared_ptr<Fact> out) const = 0;

        /**
         * @brief By default, a data-flow analysis does not have edge transfer, i.e.,
         * does not need to perform transfer for any edges.
         * @param edge the edge to check
         * @return true if this analysis needs to perform transfer for given edge, otherwise false.
         */
        [[nodiscard]] virtual bool needTransferEdge(std::shared_ptr<graph::CFGEdge> edge) const {
            return false;
        }

        /**
         * @brief Edge Transfer function for this analysis.
         * Note that this function should NOT modify nodeFact.
         * @param edge     the edge that the transfer function is applied on
         * @param nodeFact the fact of the source node of the edge. Note that
         *                 which node is the source node of an edge depends on
         *                 the direction of the analysis.
         * @return the resulting edge fact
         */
        virtual std::shared_ptr<Fact> transferEdge(std::shared_ptr<graph::CFGEdge> edge, Fact nodeFact) const {
            World::getLogger().Error("Transfer Edge is unsupported in dataflow analysis by default.");
            throw std::runtime_error("Transfer Edge is unsupported in dataflow analysis by default.");
            return nullptr;
        }

        /**
         * @return the control-flow graph that this analysis works on.
         */
        [[nodiscard]] virtual std::shared_ptr<graph::CFG> getCFG() const = 0;

        virtual ~DataFlowAnalysis() = default;

    };

    template <typename Fact>
    class AnalysisDriver: public MethodAnalysis<fact::DataflowResult<Fact>> {
    public:

        [[nodiscard]] std::shared_ptr<fact::DataflowResult<Fact>>
            analyze(std::shared_ptr<ir::IR> myIR) override
        {
            std::shared_ptr<graph::CFG> cfg = myIR->getCFG();
            std::unique_ptr<DataFlowAnalysis<Fact>> analysis = makeAnalysis(cfg);
            std::unique_ptr<solver::Solver<Fact>> solver = solver::makeSolver<Fact>();
            return solver->solve(analysis);
        }

    protected:

        /**
         * @brief Creates an analysis object for given cfg.
         * @param cfg a control flow graph
         * @return a dataflow analysis object
         */
        virtual std::unique_ptr<DataFlowAnalysis<Fact>>
            makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const = 0;

        explicit AnalysisDriver(std::unique_ptr<config::AnalysisConfig>& analysisConfig)
                :MethodAnalysis<fact::DataflowResult<Fact>>(analysisConfig)
        {

        }

    };

} // analysis

#endif //STATIC_ANALYZER_DATAFLOWANALYSIS_H
