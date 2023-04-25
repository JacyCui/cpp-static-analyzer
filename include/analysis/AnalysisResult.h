#ifndef STATIC_ANALYZER_ANALYSISRESULT_H
#define STATIC_ANALYZER_ANALYSISRESULT_H

#include "ir/Stmt.h"

namespace analyzer::analysis {

    /**
     * @class StmtResult
     * @brief An interface for querying analysis results of Stmt.
     * @tparam R type of analysis results
     */
    template <typename R>
    class StmtResult {
    public:

        /**
         * @brief judge the relevance of a given statement
         * @param stmt a statement
         * @return true if the stmt is relevant to this result
         */
        [[nodiscard]] virtual bool isRelevant(std::shared_ptr<ir::Stmt> stmt) const = 0;

        /**
         * @brief get the result of a given statement
         * @param stmt statement to get result
         * @return the analysis result of stmt, nullptr if it doesn't exist
         */
        [[nodiscard]] virtual std::shared_ptr<R> getResult(std::shared_ptr<ir::Stmt> stmt) const = 0;

        virtual ~StmtResult() = default;

    };

}

#endif //STATIC_ANALYZER_ANALYSISRESULT_H
