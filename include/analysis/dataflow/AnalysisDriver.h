#ifndef STATIC_ANALYZER_ANALYSISDRIVER_H
#define STATIC_ANALYZER_ANALYSISDRIVER_H

#include "analysis/Analysis.h"

#include "analysis/dataflow/solver/Solver.h"

namespace analyzer::analysis::dataflow {

    template <typename Fact>
    class AnalysisDriver: public MethodAnalysis<fact::DataflowResult<Fact>> {
    public:

        [[nodiscard]] std::shared_ptr<fact::DataflowResult<Fact>>
        analyze(std::shared_ptr<ir::IR> myIR) override
        {
            World::getLogger().Progress("Start dataflow analysis: " + this->analysisConfig->getDescription());
            std::shared_ptr<graph::CFG> cfg = myIR->getCFG();
            World::getLogger().Info("Getting dataflow analysis algorithm ...");
            std::unique_ptr<DataflowAnalysis<Fact>> dataflowAnalysis = makeAnalysis(cfg);
            World::getLogger().Info("Getting dataflow analysis solver (worklist solver by default) ...");
            std::unique_ptr<solver::Solver<Fact>> mySolver = solver::makeSolver<Fact>();
            World::getLogger().Info("Solving the dataflow analysis ...");
            std::shared_ptr<fact::DataflowResult<Fact>> result = mySolver->solve(dataflowAnalysis);
            World::getLogger().Success("Finish dataflow analysis: " + this->analysisConfig->getDescription());
            return result;
        }

    protected:

        /**
         * @brief Creates an analysis object for given cfg.
         * @param cfg a control flow graph
         * @return a dataflow analysis object
         */
        virtual std::unique_ptr<DataflowAnalysis<Fact>>
            makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const = 0;

        explicit AnalysisDriver(std::unique_ptr<config::AnalysisConfig>& analysisConfig)
            :MethodAnalysis<fact::DataflowResult<Fact>>(analysisConfig)
        {

        }

    };

} // dataflow

#endif //STATIC_ANALYZER_ANALYSISDRIVER_H
