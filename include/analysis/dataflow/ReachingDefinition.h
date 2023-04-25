#ifndef STATIC_ANALYZER_REACHINGDEFINITION_H
#define STATIC_ANALYZER_REACHINGDEFINITION_H

#include <memory>

#include "analysis/dataflow/AnalysisDriver.h"
#include "analysis/dataflow/fact/SetFact.h"

namespace analyzer::analysis::dataflow {

    /**
     * @class ReachingDefinition
     * @brief reaching definition analysis
     */
    class ReachingDefinition: public AnalysisDriver<fact::SetFact<ir::Stmt>> {
    public:

        /**
         * @brief constructor for reaching definition analysis
         * @param analysisConfig the analysis configuration
         */
        explicit ReachingDefinition(std::unique_ptr<config::AnalysisConfig>& analysisConfig);

    protected:

        [[nodiscard]] std::unique_ptr<DataflowAnalysis<fact::SetFact<ir::Stmt>>>
            makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const override;

    };


} // dataflow


#endif //STATIC_ANALYZER_REACHINGDEFINITION_H
