#ifndef STATIC_ANALYZER_NODERESULT_H
#define STATIC_ANALYZER_NODERESULT_H

#include "analysis/AnalysisResult.h"

namespace analyzer::analysis::dataflow::fact {

    /**
     * @brief An interface for querying data-flow results.
     * @class NodeResult
     * @tparam Fact the type of dataflow fact
     */
    template <typename Fact>
    class NodeResult: public StmtResult<Fact> {
    public:

        /**
         * @brief get the flowing-in fact of a given node
         * @param node a statement to get the in fact
         * @return a dataflow fact, nullptr if not exist
         */
        [[nodiscard]] virtual std::shared_ptr<Fact> getInFact(std::shared_ptr<ir::Stmt> node) const = 0;

        /**
         * @brief get the flowing-out fact of a given node
         * @param node a statement to get the out fact
         * @return a dataflow fact, nullptr if not exist
         */
        [[nodiscard]] virtual std::shared_ptr<Fact> getOutFact(std::shared_ptr<ir::Stmt> node) const = 0;

        /**
         * @brief typically, all statements are relevant in NodeResult
         * @param node an unused argument
         * @return true
         */
        [[nodiscard]] bool isRelevant(std::shared_ptr<ir::Stmt> node) const override
        {
            return true;
        }

        [[nodiscard]] std::shared_ptr<Fact> getResult(std::shared_ptr<ir::Stmt> stmt) const override
        {
            return getOutFact(stmt);
        }

    };

} // fact

#endif //STATIC_ANALYZER_NODERESULT_H
