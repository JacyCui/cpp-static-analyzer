#include "analysis/Analysis.h"

namespace analyzer::analysis {

    const std::unique_ptr<config::AnalysisConfig>& Analysis::getAnalysisConfig() const
    {
        return analysisConfig;
    }

}

