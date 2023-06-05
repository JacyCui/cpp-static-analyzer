#include "analysis/dataflow/LiveVariable.h"

namespace analyzer::analysis::dataflow {

    LiveVariable::LiveVariable(std::unique_ptr<config::AnalysisConfig>& analysisConfig)
        :AnalysisDriver<fact::SetFact<ir::Var>>(analysisConfig)
    {

    }

    std::unique_ptr<DataflowAnalysis<fact::SetFact<ir::Var>>>
        LiveVariable::makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const
    {
        class Analysis: public AbstractDataflowAnalysis<fact::SetFact<ir::Var>> {
        public:

            [[nodiscard]] bool isForward() const override
            {
                return false;
            }

            [[nodiscard]] std::shared_ptr<fact::SetFact<ir::Var>> newBoundaryFact() const override
            {
                return newInitialFact();
            }

            [[nodiscard]] std::shared_ptr<fact::SetFact<ir::Var>> newInitialFact() const override
            {
                return std::make_shared<fact::SetFact<ir::Var>>();
            }

            void meetInto(std::shared_ptr<fact::SetFact<ir::Var>> fact,
                          std::shared_ptr<fact::SetFact<ir::Var>> target) const override
            {
                target->unionN(fact);
            }

            [[nodiscard]] bool transferNode(
                    std::shared_ptr<ir::Stmt> stmt,
                    std::shared_ptr<fact::SetFact<ir::Var>> in,
                    std::shared_ptr<fact::SetFact<ir::Var>> out) const override
            {
                std::shared_ptr<fact::SetFact<ir::Var>> oldIn = in->copy();
                in->setSetFact(out);
                for (const std::shared_ptr<ir::Var>& def : stmt->getDefs()) {
                    in->remove(def);
                }
                for (const std::shared_ptr<ir::Var>& use : stmt->getUses()) {
                    in->add(use);
                }
                return !in->equalsTo(oldIn);
            }

            [[nodiscard]] std::shared_ptr<fact::DataflowResult<fact::SetFact<ir::Var>>>
                getResult() const override
            {
                return std::make_shared<fact::DataflowResult<fact::SetFact<ir::Var>>>();
            }

            explicit Analysis(const std::shared_ptr<graph::CFG>& myCFG)
                    : AbstractDataflowAnalysis<fact::SetFact<ir::Var>>(myCFG)
            {

            }

        };

        return std::make_unique<Analysis>(cfg);
    }

}
