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

    std::shared_ptr<graph::CFG> cfg = ir2->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir2->getStmts()) {
        al::World::getLogger().Debug(s->str());
        stmtMap.emplace(s->str(), s);
    }
    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    CHECK_EQ(ir2->getVars().size(), 3);
    for (const std::shared_ptr<air::Var>& v : ir2->getVars()) {
        varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("int y = 0;");
    std::shared_ptr<air::Stmt> s2 = stmtMap.at("int i = 0;");
    std::shared_ptr<air::Stmt> s3 = stmtMap.at("i < x");
    std::shared_ptr<air::Stmt> s4 = stmtMap.at("++i");
    std::shared_ptr<air::Stmt> s5 = stmtMap.at("y += 20");
    std::shared_ptr<air::Stmt> s6 = stmtMap.at("y");
    std::shared_ptr<air::Stmt> s7 = stmtMap.at("return y");

    std::shared_ptr<air::Var> v1 = varMap.at("y");
    std::shared_ptr<air::Var> v2 = varMap.at("i");
    std::shared_ptr<air::Var> v3 = varMap.at("x");

    std::shared_ptr<dfact::DataflowResult<dfact::SetFact<air::Var>>> result = lv->analyze(ir2);

    CHECK(result->getOutFact(cfg->getExit())->isEmpty());
    CHECK(result->getInFact(cfg->getExit())->isEmpty());
    CHECK(result->getOutFact(s7)->isEmpty());
    CHECK(result->getInFact(s7)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1})));
    CHECK(result->getOutFact(s6)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1})));
    CHECK(result->getInFact(s6)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1})));
    CHECK(result->getOutFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3})));
    CHECK(result->getInFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3})));
    CHECK(result->getOutFact(s4)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3})));
    CHECK(result->getInFact(s4)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3})));
    CHECK(result->getOutFact(s3)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3})));
    CHECK(result->getInFact(s3)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3})));
    CHECK(result->getOutFact(s2)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3})));
    CHECK(result->getInFact(s2)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3})));
    CHECK(result->getOutFact(s1)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3})));
    CHECK(result->getInFact(s1)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v3})));
    CHECK(result->getOutFact(cfg->getEntry())->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v3})));
    CHECK(result->getInFact(cfg->getEntry())->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v3})));

    al::World::getLogger().Success("Finish testing live variable example loop ...");

}

TEST_CASE_FIXTURE(LiveVarTestFixture, "testLiveVarCaseLoopBranch"
                                      * doctest::description("testing live variable example loopBranch ...")) {

    al::World::getLogger().Progress("Testing live variable example loopBranch ...");

    std::shared_ptr<graph::CFG> cfg = ir3->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir3->getStmts()) {
        al::World::getLogger().Debug(s->str());
        stmtMap.emplace(s->str(), s);
    }
    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    CHECK_EQ(ir3->getVars().size(), 5);
    for (const std::shared_ptr<air::Var>& v : ir3->getVars()) {
        varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("int a;");
    std::shared_ptr<air::Stmt> s2 = stmtMap.at("int i = m - 1;");
    std::shared_ptr<air::Stmt> s3 = stmtMap.at("i < k");
    std::shared_ptr<air::Stmt> s4 = stmtMap.at("i++");
    std::shared_ptr<air::Stmt> s5 = stmtMap.at("a = n");
    std::shared_ptr<air::Stmt> s6 = stmtMap.at("a = a + i");
    std::shared_ptr<air::Stmt> s7 = stmtMap.at("a");
    std::shared_ptr<air::Stmt> s8 = stmtMap.at("return a");

    std::shared_ptr<air::Var> v1 = varMap.at("a");
    std::shared_ptr<air::Var> v2 = varMap.at("i");
    std::shared_ptr<air::Var> v3 = varMap.at("k");
    std::shared_ptr<air::Var> v4 = varMap.at("m");
    std::shared_ptr<air::Var> v5 = varMap.at("n");

    std::shared_ptr<dfact::DataflowResult<dfact::SetFact<air::Var>>> result = lv->analyze(ir3);

    CHECK(result->getOutFact(cfg->getExit())->isEmpty());
    CHECK(result->getInFact(cfg->getExit())->isEmpty());
    CHECK(result->getOutFact(s8)->isEmpty());
    CHECK(result->getInFact(s8)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1})));
    CHECK(result->getOutFact(s7)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1})));
    CHECK(result->getInFact(s7)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1})));
    CHECK(result->getOutFact(s6)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getInFact(s6)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getOutFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getInFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v2, v3, v5})));
    CHECK(result->getOutFact(s4)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getInFact(s4)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getOutFact(s3)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getInFact(s3)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getOutFact(s2)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getInFact(s2)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3, v4, v5})));
    CHECK(result->getOutFact(s1)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3, v4, v5})));
    CHECK(result->getInFact(s1)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3, v4, v5})));
    CHECK(result->getOutFact(cfg->getEntry())->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3, v4, v5})));
    CHECK(result->getInFact(cfg->getEntry())->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v3, v4, v5})));

    al::World::getLogger().Success("Finish testing live variable example loopBranch ...");

}

TEST_CASE_FIXTURE(LiveVarTestFixture, "testLiveVarCaseBranchLoop"
                                      * doctest::description("testing live variable example branchLoop ...")) {

    al::World::getLogger().Progress("Testing live variable example branchLoop ...");

    std::shared_ptr<graph::CFG> cfg = ir4->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir4->getStmts()) {
        al::World::getLogger().Debug(s->str());
        stmtMap.emplace(s->str(), s);
    }
    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    CHECK_EQ(ir4->getVars().size(), 5);
    for (const std::shared_ptr<air::Var>& v : ir4->getVars()) {
        varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("x");
    std::shared_ptr<air::Stmt> s2 = stmtMap.at("y");
    std::shared_ptr<air::Stmt> s3 = stmtMap.at("z");
    std::shared_ptr<air::Stmt> s4 = stmtMap.at("x = 1");
    std::shared_ptr<air::Stmt> s5 = stmtMap.at("y = 2");
    std::shared_ptr<air::Stmt> s6 = stmtMap.at("c > 0");
    std::shared_ptr<air::Stmt> s7 = stmtMap.at("x = y + 1");
    std::shared_ptr<air::Stmt> s8 = stmtMap.at("y = 2 * z");
    std::shared_ptr<air::Stmt> s9 = stmtMap.at("d");
    std::shared_ptr<air::Stmt> s10 = stmtMap.at("x = y + z");
    std::shared_ptr<air::Stmt> s11 = stmtMap.at("z = 1");
    std::shared_ptr<air::Stmt> s12 = stmtMap.at("c < 20");
    std::shared_ptr<air::Stmt> s13 = stmtMap.at("nop");
    std::shared_ptr<air::Stmt> s14 = stmtMap.at("z = x");

    std::shared_ptr<air::Var> v1 = varMap.at("c");
    std::shared_ptr<air::Var> v2 = varMap.at("d");
    std::shared_ptr<air::Var> v3 = varMap.at("x");
    std::shared_ptr<air::Var> v4 = varMap.at("y");
    std::shared_ptr<air::Var> v5 = varMap.at("z");

    std::shared_ptr<dfact::DataflowResult<dfact::SetFact<air::Var>>> result = lv->analyze(ir4);

    CHECK(result->getOutFact(cfg->getExit())->isEmpty());
    CHECK(result->getInFact(cfg->getExit())->isEmpty());
    CHECK(result->getOutFact(s14)->isEmpty());
    CHECK(result->getInFact(s14)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v3})));
    CHECK(result->getOutFact(s13)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v4, v5})));
    CHECK(result->getInFact(s13)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v4, v5})));
    CHECK(result->getOutFact(s12)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v4, v5})));
    CHECK(result->getInFact(s12)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v4, v5})));
    CHECK(result->getOutFact(s11)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v4, v5})));
    CHECK(result->getInFact(s11)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v4})));
    CHECK(result->getOutFact(s10)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v4})));
    CHECK(result->getInFact(s10)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v4, v5})));
    CHECK(result->getOutFact(s9)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v4, v5})));
    CHECK(result->getInFact(s9)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v4, v5})));
    CHECK(result->getOutFact(s8)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v4, v5})));
    CHECK(result->getInFact(s8)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getOutFact(s7)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getInFact(s7)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v4, v5})));
    CHECK(result->getOutFact(s6)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v4, v5})));
    CHECK(result->getInFact(s6)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v4, v5})));
    CHECK(result->getOutFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v4, v5})));
    CHECK(result->getInFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getOutFact(s4)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v3, v5})));
    CHECK(result->getInFact(s4)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v5})));
    CHECK(result->getOutFact(s3)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v5})));
    CHECK(result->getInFact(s3)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v5})));
    CHECK(result->getOutFact(s2)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v5})));
    CHECK(result->getInFact(s2)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v5})));
    CHECK(result->getOutFact(s1)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v5})));
    CHECK(result->getInFact(s1)->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v5})));
    CHECK(result->getOutFact(cfg->getEntry())->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v5})));
    CHECK(result->getInFact(cfg->getEntry())->equalsTo(
            std::make_shared<dfact::SetFact<air::Var>>(std::unordered_set{v1, v2, v5})));

    al::World::getLogger().Success("Finish testing live variable example branchLoop ...");

}

TEST_SUITE_END();
