#ifndef STATIC_ANALYZER_ANALYSIS_H
#define STATIC_ANALYZER_ANALYSIS_H

#include <memory>

#include "config/AnalysisConfig.h"
#include "ir/IR.h"

namespace analyzer::analysis {

    /**
     * @class Analysis
     * @brief abstract base class for all analysis
     */
    class Analysis {
    public:

        [[nodiscard]] const std::unique_ptr<config::AnalysisConfig>& getAnalysisConfig() const;

        virtual ~Analysis() = default;

    protected:

        /**
         * @brief construct an analysis from its configuration
         * @param analysisConfig a unique configuration object
         */
        explicit Analysis(std::unique_ptr<config::AnalysisConfig>& analysisConfig)
            :analysisConfig(std::move(analysisConfig))
        {

        }

        std::unique_ptr<config::AnalysisConfig> analysisConfig; ///< the configuration of this analysis

    };

    /**
     * @class MethodAnalysis
     * @brief abstract base class for all method analyses, i.e. intra-procedural analyses.
     * @tparam R the result type
     */
    template <typename R>
    class MethodAnalysis: public Analysis {
    public:

        /**
         * @brief analyze an intermediate representation to get the result
         * @param myIR the ir to be analyzed
         * @return the method analysis result
         */
        [[nodiscard]] virtual std::shared_ptr<R> analyze(std::shared_ptr<ir::IR> myIR) = 0;

    protected:

        explicit MethodAnalysis(std::unique_ptr<config::AnalysisConfig>& analysisConfig)
            :Analysis(analysisConfig)
        {

        }

    };

} // analysis

#endif //STATIC_ANALYZER_ANALYSIS_H
