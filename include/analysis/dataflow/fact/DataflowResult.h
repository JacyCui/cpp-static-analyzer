#ifndef STATIC_ANALYZER_DATAFLOWRESULT_H
#define STATIC_ANALYZER_DATAFLOWRESULT_H

#include <unordered_map>

#include "analysis/dataflow/fact/NodeResult.h"

namespace analyzer::analysis::dataflow::fact {

    template <typename Fact>
    class DataflowResult: public NodeResult<Fact> {
    public:

        [[nodiscard]] std::shared_ptr<Fact> getInFact(std::shared_ptr<ir::Stmt> node) const override
        {
            if (inFacts.find(node) != inFacts.end()) {
                return inFacts.at(node);
            }
            return nullptr;
        }

        [[nodiscard]] std::shared_ptr<Fact> getOutFact(std::shared_ptr<ir::Stmt> node) const override
        {
            if (outFacts.find(node) != outFacts.end()) {
                return outFacts.at(node);
            }
            return nullptr;
        }

        /**
         * @brief Associates a dataflow fact with a node as its flowing-in fact.
         * @param node a statement
         * @param fact a dataflow fact
         */
        void setInFact(std::shared_ptr<ir::Stmt> node, std::shared_ptr<Fact> fact) {
            inFacts.insert_or_assign(node, fact);
        }

        /**
         * @brief Associates a dataflow fact with a node as its flowing-out fact.
         * @param node a statement
         * @param fact a dataflow fact
         */
        void setOutFact(std::shared_ptr<ir::Stmt> node, std::shared_ptr<Fact> fact) {
            outFacts.insert_or_assign(node, fact);
        }

        /**
         * @brief construct a predefined dataflow result
         * @param inFacts all in-flowing facts
         * @param outFacts all out-flowing facts
         */
        DataflowResult(std::unordered_map<std::shared_ptr<ir::Stmt>, Fact> inFacts,
            std::unordered_map<std::shared_ptr<ir::Stmt>, Fact> outFacts)
            :inFacts(std::move(inFacts)), outFacts(std::move(outFacts))
        {

        }

        /**
         * @brief construct an empty dataflow result
         */
        DataflowResult()
            :DataflowResult({}, {})
        {

        }

    private:

        std::unordered_map<std::shared_ptr<ir::Stmt>, std::shared_ptr<Fact>> inFacts; ///< in-flowing facts of each statement

        std::unordered_map<std::shared_ptr<ir::Stmt>, std::shared_ptr<Fact>> outFacts; ///< out-flowing facts of each statement

    };

} // fact

#endif //STATIC_ANALYZER_DATAFLOWRESULT_H
