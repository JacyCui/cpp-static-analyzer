#ifndef STATIC_ANALYZER_ANALYSISCONFIG_H
#define STATIC_ANALYZER_ANALYSISCONFIG_H

#include <string>

namespace analyzer::config {

    /**
     * @class AnalysisConfig
     * @brief configuration for an analysis
     */
    class AnalysisConfig {
    public:

        /**
         * @return the description of this analysis, just for human reading
         */
        [[nodiscard]] virtual const std::string& getDescription() const = 0;

        virtual ~AnalysisConfig() = default;

    };

    class DefaultAnalysisConfig final: public AnalysisConfig {
    public:

        [[nodiscard]] const std::string& getDescription() const override;

        /**
         * @brief construct a default analysis config that only has a description
         * @param description the description of the corresponding analysis
         */
        explicit DefaultAnalysisConfig(std::string description = "");

    private:

        std::string description; ///< the description of this analysis

    };

}

#endif //STATIC_ANALYZER_ANALYSISCONFIG_H
