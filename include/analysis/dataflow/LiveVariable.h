#ifndef CPP_STATIC_ANALYZER_LIVEVARIABLE_H
#define CPP_STATIC_ANALYZER_LIVEVARIABLE_H

#include <memory>

#include "analysis/dataflow/AnalysisDriver.h"
#include "analysis/dataflow/fact/SetFact.h"

namespace analyzer::analysis::dataflow {

    /**
     * @class LiveVariable
     * @brief live variable analysis
     */
    class LiveVariable: public AnalysisDriver<fact::SetFact<ir::Var>> {
    public:

        /**
         * @brief constructor for live variable analysis
         * @param analysisConfig the analysis configuration
         */
        explicit LiveVariable(std::unique_ptr<config::AnalysisConfig>& analysisConfig);

    protected:

        [[nodiscard]] std::unique_ptr<DataflowAnalysis<fact::SetFact<ir::Var>>>
            makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const override;

};


} // dataflow

#endif //CPP_STATIC_ANALYZER_LIVEVARIABLE_H
