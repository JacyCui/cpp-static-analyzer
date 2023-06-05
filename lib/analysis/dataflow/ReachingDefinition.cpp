#include "analysis/dataflow/ReachingDefinition.h"

namespace analyzer::analysis::dataflow {

    ReachingDefinition::ReachingDefinition(std::unique_ptr<config::AnalysisConfig>& analysisConfig)
        :AnalysisDriver<fact::SetFact<ir::Stmt>>(analysisConfig)
    {

    }

    std::unique_ptr<DataflowAnalysis<fact::SetFact<ir::Stmt>>>
        ReachingDefinition::makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const
    {

        class Analysis: public AbstractDataflowAnalysis<fact::SetFact<ir::Stmt>> {
        public:

            [[nodiscard]] bool isForward() const override
            {
                return true;
            }

            [[nodiscard]] std::shared_ptr<fact::SetFact<ir::Stmt>> newBoundaryFact() const override
            {
                return newInitialFact();
            }

            [[nodiscard]] std::shared_ptr<fact::SetFact<ir::Stmt>> newInitialFact() const override
            {
                return std::make_shared<fact::SetFact<ir::Stmt>>();
            }

            void meetInto(std::shared_ptr<fact::SetFact<ir::Stmt>> fact,
                                        std::shared_ptr<fact::SetFact<ir::Stmt>> target) const override
            {
                target->unionN(fact);
            }

            [[nodiscard]] bool transferNode(
                    std::shared_ptr<ir::Stmt> stmt,
                    std::shared_ptr<fact::SetFact<ir::Stmt>> in,
                    std::shared_ptr<fact::SetFact<ir::Stmt>> out) const override
            {
                std::shared_ptr<fact::SetFact<ir::Stmt>> oldOut = out->copy();
                out->setSetFact(in);
                for (const std::shared_ptr<ir::Var>& def : stmt->getDefs()) {
                    out->removeAll(defs.at(def));
                }
                if (!stmt->getDefs().empty()) {
                    out->add(stmt);
                }
                return !out->equalsTo(oldOut);
            }

            [[nodiscard]] std::shared_ptr<fact::DataflowResult<fact::SetFact<ir::Stmt>>>
                    getResult() const override
            {
                return std::make_shared<fact::DataflowResult<fact::SetFact<ir::Stmt>>>();
            }

            explicit Analysis(const std::shared_ptr<graph::CFG>& myCFG)
                : AbstractDataflowAnalysis<fact::SetFact<ir::Stmt>>(myCFG)
            {
                computeDefs(cfg->getIR());
            }

        private:

            void computeDefs(const std::shared_ptr<ir::IR>& myIR)
            {
                for (const std::shared_ptr<ir::Var>& var : myIR->getVars()) {
                    defs.emplace(var, std::make_shared<fact::SetFact<ir::Stmt>>());
                }
                for (const std::shared_ptr<ir::Stmt>& stmt : myIR->getStmts()) {
                    for (const std::shared_ptr<ir::Var>& var : stmt->getDefs()) {
                        defs.at(var)->add(stmt);
                    }
                }
            }

            std::unordered_map<std::shared_ptr<ir::Var>, std::shared_ptr<fact::SetFact<ir::Stmt>>> defs;

        };

        return std::make_unique<Analysis>(cfg);
    }

}
