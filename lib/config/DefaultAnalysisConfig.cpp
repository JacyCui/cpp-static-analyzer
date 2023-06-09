#include <utility>

#include "config/AnalysisConfig.h"

namespace analyzer::config {

    DefaultAnalysisConfig::DefaultAnalysisConfig(std::string description)
        :description(std::move(description))
    {

    }

    const std::string& DefaultAnalysisConfig::getDescription() const
    {
        return description;
    }

}
