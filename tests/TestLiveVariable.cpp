#include "doctest.h"

#include "World.h"
#include "analysis/dataflow/LiveVariable.h"

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace graph = al::analysis::graph;
namespace df = al::analysis::dataflow;
namespace dfact = al::analysis::dataflow::fact;

class LiveVarTestFixture {
protected:
    std::shared_ptr<air::IR> ir1, ir2, ir3, ir4;
    std::unique_ptr<df::LiveVariable> lv;
public:
    LiveVarTestFixture() {
        al::World::initialize("resources/dataflow/LiveVar");
        const al::World& world = al::World::get();
        ir1 = world.getMethodBySignature("int LiveVar::ifElse(int, int, int)")->getIR();
        ir2 = world.getMethodBySignature("int LiveVar::loop(int)")->getIR();
        ir3 = world.getMethodBySignature("int LiveVar::loopBranch(int, int, int)")->getIR();
        ir4 = world.getMethodBySignature("void LiveVar::branchLoop(int, _Bool)")->getIR();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig
                = std::make_unique<cf::DefaultAnalysisConfig>("live variable analysis");
        lv = std::make_unique<df::LiveVariable>(analysisConfig);

        CHECK_FALSE(analysisConfig);
    }
};

TEST_SUITE_BEGIN("testLiveVariable");

TEST_CASE_FIXTURE(LiveVarTestFixture, "testLiveVarCaseIfElse"
    * doctest::description("testing live variable example ifElse ...")) {

    al::World::getLogger().Progress("Testing live variable example ifElse ...");

    std::shared_ptr<graph::CFG> cfg = ir1->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir1->getStmts()) {
        al::World::getLogger().Debug(s->str());
        stmtMap.emplace(s->str(), s);
    }
    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    CHECK_EQ(ir1->getVars().size(), 4);
    for (const std::shared_ptr<air::Var>& v : ir1->getVars()) {
        varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("int x = m;");
    std::shared_ptr<air::Stmt> s2 = stmtMap.at("n > 0");
    std::shared_ptr<air::Stmt> s3 = stmtMap.at("x + n");
    std::shared_ptr<air::Stmt> s4 = stmtMap.at("return x + n");
    std::shared_ptr<air::Stmt> s5 = stmtMap.at("k + n");
    std::shared_ptr<air::Stmt> s6 = stmtMap.at("return k + n");

    std::shared_ptr<air::Var> v1 = varMap.at("x");
    std::shared_ptr<air::Var> v2 = varMap.at("m");
    std::shared_ptr<air::Var> v3 = varMap.at("n");
    std::shared_ptr<air::Var> v4 = varMap.at("k");

    std::shared_ptr<dfact::DataflowResult<dfact::SetFact<air::Var>>> result = lv->analyze(ir1);

    CHECK(result->getOutFact(cfg->getExit())->isEmpty());
    CHECK(result->getInFact(cfg->getExit())->isEmpty());
    CHECK(result->getOutFact(s6)->isEmpty());
    CHECK(result->getOutFact(s4)->isEmpty());
    CHECK(result->getInFact(s6)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v3, v4})));
    CHECK(result->getOutFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v3, v4})));
    CHECK(result->getInFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v3, v4})));
    CHECK(result->getInFact(s4)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3})));
    CHECK(result->getOutFact(s3)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3})));
    CHECK(result->getInFact(s3)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3})));
    CHECK(result->getOutFact(s2)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3, v4})));
    CHECK(result->getInFact(s2)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3, v4})));
    CHECK(result->getOutFact(s1)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3, v4})));
    CHECK(result->getInFact(s1)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v2, v3, v4})));
    CHECK(result->getOutFact(cfg->getEntry())->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v2, v3, v4})));
    CHECK(result->getInFact(cfg->getEntry())->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v2, v3, v4})));

    al::World::getLogger().Success("Finish testing live variable example ifElse ...");

}

TEST_CASE_FIXTURE(LiveVarTestFixture, "testLiveVarCaseLoop"
                                      * doctest::description("testing live variable example loop ...")) {

    al::World::getLogger().Progress("Testing live variable example loop ...");



    al::World::getLogger().Success("Finish testing live variable example loop ...");

}

TEST_CASE_FIXTURE(LiveVarTestFixture, "testLiveVarCaseLoopBranch"
                                      * doctest::description("testing live variable example loopBranch ...")) {

    al::World::getLogger().Progress("Testing live variable example loopBranch ...");



    al::World::getLogger().Success("Finish testing live variable example loopBranch ...");

}

TEST_CASE_FIXTURE(LiveVarTestFixture, "testLiveVarCaseBranchLoop"
                                      * doctest::description("testing live variable example branchLoop ...")) {

    al::World::getLogger().Progress("Testing live variable example branchLoop ...");



    al::World::getLogger().Success("Finish testing live variable example branchLoop ...");

}

TEST_SUITE_END();
