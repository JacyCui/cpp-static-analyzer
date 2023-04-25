#ifndef STATIC_ANALYZER_DataflowAnalysis_H
#define STATIC_ANALYZER_DataflowAnalysis_H

#include "World.h"
#include "analysis/graph/CFG.h"
#include "analysis/Analysis.h"
#include "analysis/dataflow/fact/DataflowResult.h"

namespace analyzer::analysis::dataflow {

    /**
     * @brief Template interface for defining data-flow analysis.
     * @tparam Fact type of data-flow facts
     */
    template <typename Fact>
    class DataflowAnalysis {
    public:

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
        [[nodiscard]] virtual bool needTransferEdge(std::shared_ptr<graph::CFGEdge> edge) const = 0;

        /**
         * @brief Edge Transfer function for this analysis.
         * Note that this function should NOT modify nodeFact.
         * @param edge     the edge that the transfer function is applied on
         * @param nodeFact the fact of the source node of the edge. Note that
         *                 which node is the source node of an edge depends on
         *                 the direction of the analysis.
         * @return the resulting edge fact
         */
        [[nodiscard]] virtual std::shared_ptr<Fact> transferEdge
            (std::shared_ptr<graph::CFGEdge> edge, std::shared_ptr<Fact> nodeFact) const = 0;

        /**
         * @return the control-flow graph that this analysis works on.
         */
        [[nodiscard]] virtual std::shared_ptr<graph::CFG> getCFG() const = 0;

        virtual ~DataflowAnalysis() = default;

    };

    /**
     * @class AbstractDataflowAnalysis
     * @brief An abstract dataflow analysis that implements some general logic
     * @tparam Fact dataflow fact type
     */
    template <typename Fact>
    class AbstractDataflowAnalysis: public DataflowAnalysis<Fact> {
    public:

        [[nodiscard]] bool needTransferEdge(std::shared_ptr<graph::CFGEdge> edge) const override
        {
            return false;
        }

        [[nodiscard]] std::shared_ptr<Fact> transferEdge
            (std::shared_ptr<graph::CFGEdge> edge, std::shared_ptr<Fact> nodeFact) const override
        {
            World::getLogger().Error("Transfer Edge is unsupported in dataflow analysis by default.");
            throw std::runtime_error("Transfer Edge is unsupported in dataflow analysis by default.");
        }

        [[nodiscard]] std::shared_ptr<graph::CFG> getCFG() const override
        {
            return cfg;
        }

    protected:

        /**
         * @brief construct a dataflow analysis from a given cfg
         * @param cfg the cfg to be analyzed
         */
        explicit AbstractDataflowAnalysis(const std::shared_ptr<graph::CFG>& cfg)
            :cfg(cfg)
        {

        }

        std::shared_ptr<graph::CFG> cfg; ///< the cfg to be analyzed

    };


} // dataflow

#endif //STATIC_ANALYZER_DataflowAnalysis_H
