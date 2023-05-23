#include "doctest.h"

#include "World.h"
#include "analysis/dataflow/ReachingDefinition.h"

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace graph = al::analysis::graph;
namespace df = al::analysis::dataflow;
namespace dfact = al::analysis::dataflow::fact;

class ReachDefTestFixture {
protected:
    std::shared_ptr<air::IR> ir1, ir2;
    std::unique_ptr<df::ReachingDefinition> rd;
public:
    ReachDefTestFixture() {
        al::World::initialize("resources/dataflow/ReachDef");
        const al::World& world = al::World::get();
        ir1 = world.getMethodBySignature("int ReachDef::foo(int, int, int)")->getIR();
        ir2 = world.getMethodBySignature("int ReachDef::loop(int, int)")->getIR();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig
                = std::make_unique<cf::DefaultAnalysisConfig>("reaching definition analysis");
        rd = std::make_unique<df::ReachingDefinition>(analysisConfig);

        CHECK_FALSE(analysisConfig);
    }
};


TEST_SUITE_BEGIN("testReachingDefinition");

TEST_CASE_FIXTURE(ReachDefTestFixture, "testDataflowPreparation"
    * doctest::description("testing dataflow preparation work ...")) {

    al::World::getLogger().Progress("Testing dataflow preparation work ...");

    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap1;
    for (const std::shared_ptr<air::Stmt>& s : ir1->getStmts()) {
        al::World::getLogger().Debug(s->str());
        stmtMap1.emplace(s->str(), s);
    }
    std::shared_ptr<air::Stmt> s11 = stmtMap1.at("int x;");
    std::shared_ptr<air::Stmt> s12 = stmtMap1.at("a > 0");
    std::shared_ptr<air::Stmt> s13 = stmtMap1.at("x = a");
    std::shared_ptr<air::Stmt> s14 = stmtMap1.at("x = b");
    std::shared_ptr<air::Stmt> s15 = stmtMap1.at("int y = x;");
    std::shared_ptr<air::Stmt> s16 = stmtMap1.at("x = c");
    std::shared_ptr<air::Stmt> s17 = stmtMap1.at("x");
    std::shared_ptr<air::Stmt> s18 = stmtMap1.at("return x");

    std::shared_ptr<graph::CFG> cfg1 = ir1->getCFG();
    CHECK_EQ(cfg1->getEdgeNum(), 10);
    CHECK(cfg1->hasEdge(cfg1->getEntry(), s11));
    CHECK(cfg1->hasEdge(s11, s12));
    CHECK(cfg1->hasEdge(s12, s13));
    CHECK(cfg1->hasEdge(s12, s14));
    CHECK(cfg1->hasEdge(s13, s15));
    CHECK(cfg1->hasEdge(s14, s15));
    CHECK(cfg1->hasEdge(s15, s16));
    CHECK(cfg1->hasEdge(s16, s17));
    CHECK(cfg1->hasEdge(s17, s18));
    CHECK(cfg1->hasEdge(s18, cfg1->getExit()));

    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap2;
    for (const std::shared_ptr<air::Stmt>& s : ir2->getStmts()) {
        al::World::getLogger().Debug(s->str());
        stmtMap2.emplace(s->str(), s);
    }
    std::shared_ptr<air::Stmt> s21 = stmtMap2.at("int c;");
    std::shared_ptr<air::Stmt> s22 = stmtMap2.at("a > b");
    std::shared_ptr<air::Stmt> s23 = stmtMap2.at("c = b");
    std::shared_ptr<air::Stmt> s24 = stmtMap2.at("--a");
    std::shared_ptr<air::Stmt> s25 = stmtMap2.at("nop");
    std::shared_ptr<air::Stmt> s26 = stmtMap2.at("c");
    std::shared_ptr<air::Stmt> s27 = stmtMap2.at("return c");

    std::shared_ptr<graph::CFG> cfg2 = ir2->getCFG();
    CHECK_EQ(cfg2->getEdgeNum(), 9);
    CHECK(cfg2->hasEdge(cfg2->getEntry(), s21));
    CHECK(cfg2->hasEdge(s21, s22));
    CHECK(cfg2->hasEdge(s22, s23));
    CHECK(cfg2->hasEdge(s23, s24));
    CHECK(cfg2->hasEdge(s24, s25));
    CHECK(cfg2->hasEdge(s25, s22));
    CHECK(cfg2->hasEdge(s22, s26));
    CHECK(cfg2->hasEdge(s26, s27));
    CHECK(cfg2->hasEdge(s27, cfg2->getExit()));

    CHECK_EQ(rd->getAnalysisConfig()->getDescription(), "reaching definition analysis");

    al::World::getLogger().Success("Finish testing dataflow preparation work ...");

}

TEST_CASE_FIXTURE(ReachDefTestFixture, "testReachDefCaseFoo"
    * doctest::description("testing reaching definition example foo with if-else")) {

    al::World::getLogger().Progress("Testing reaching definition example foo with if-else ...");

    std::shared_ptr<graph::CFG> cfg = ir1->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir1->getStmts()) {
        al::World::getLogger().Debug(s->str());
        stmtMap.emplace(s->str(), s);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("int x;");
    std::shared_ptr<air::Stmt> s2 = stmtMap.at("a > 0");
    std::shared_ptr<air::Stmt> s3 = stmtMap.at("x = a");
    std::shared_ptr<air::Stmt> s4 = stmtMap.at("x = b");
    std::shared_ptr<air::Stmt> s5 = stmtMap.at("int y = x;");
    std::shared_ptr<air::Stmt> s6 = stmtMap.at("x = c");
    std::shared_ptr<air::Stmt> s7 = stmtMap.at("x");
    std::shared_ptr<air::Stmt> s8 = stmtMap.at("return x");

    std::shared_ptr<dfact::DataflowResult<dfact::SetFact<air::Stmt>>> result = rd->analyze(ir1);

    CHECK(result->getInFact(cfg->getEntry())->isEmpty());
    CHECK(result->getOutFact(cfg->getEntry())->isEmpty());
    CHECK(result->getInFact(s1)->isEmpty());
    CHECK(result->getOutFact(s1)->isEmpty());
    CHECK(result->getInFact(s2)->isEmpty());
    CHECK(result->getOutFact(s2)->isEmpty());
    CHECK(result->getInFact(s3)->isEmpty());
    CHECK(result->getOutFact(s3)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3})));
    CHECK(result->getInFact(s4)->isEmpty());
    CHECK(result->getOutFact(s4)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s4})));
    CHECK(result->getInFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getOutFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4, s5})));
    CHECK(result->getInFact(s6)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4, s5})));
    CHECK(result->getOutFact(s6)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s5, s6})));
    CHECK(result->getInFact(s7)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s5, s6})));
    CHECK(result->getOutFact(s7)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s5, s6})));
    CHECK(result->getInFact(s8)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s5, s6})));
    CHECK(result->getOutFact(s8)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s5, s6})));
    CHECK(result->getInFact(cfg->getExit())->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s5, s6})));
    CHECK(result->getOutFact(cfg->getExit())->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s5, s6})));

    for (const std::shared_ptr<air::Stmt>& s :
        {cfg->getEntry(), s1, s2, s3, s4, s5, s6, s7, s8, cfg->getExit()}) {
        CHECK(result->getResult(s)->equalsTo(result->getOutFact(s)));
    }

    al::World::getLogger().Success("Finish testing reaching definition example foo with if-else ...");

}

TEST_CASE_FIXTURE(ReachDefTestFixture, "testReachDefCaseLoop"
    * doctest::description("testing reaching definition example loop with while")) {

    al::World::getLogger().Progress("Testing reaching definition example loop with while ...");

    std::shared_ptr<graph::CFG> cfg = ir2->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir2->getStmts()) {
        al::World::getLogger().Debug(s->str());
        stmtMap.emplace(s->str(), s);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("int c;");
    std::shared_ptr<air::Stmt> s2 = stmtMap.at("a > b");
    std::shared_ptr<air::Stmt> s3 = stmtMap.at("c = b");
    std::shared_ptr<air::Stmt> s4 = stmtMap.at("--a");
    std::shared_ptr<air::Stmt> s5 = stmtMap.at("nop");
    std::shared_ptr<air::Stmt> s6 = stmtMap.at("c");
    std::shared_ptr<air::Stmt> s7 = stmtMap.at("return c");

    std::shared_ptr<dfact::DataflowResult<dfact::SetFact<air::Stmt>>> result = rd->analyze(ir2);

    CHECK(result->getInFact(cfg->getEntry())->isEmpty());
    CHECK(result->getOutFact(cfg->getEntry())->isEmpty());
    CHECK(result->getInFact(s1)->isEmpty());
    CHECK(result->getOutFact(s1)->isEmpty());
    CHECK(result->getInFact(s2)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getOutFact(s2)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getInFact(s3)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getOutFact(s3)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getInFact(s4)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getOutFact(s4)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getInFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getOutFact(s5)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getInFact(s6)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getOutFact(s6)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getInFact(s7)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getOutFact(s7)->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getInFact(cfg->getExit())->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));
    CHECK(result->getOutFact(cfg->getExit())->equalsTo(
            std::make_shared<dfact::SetFact<air::Stmt>>(std::unordered_set{s3, s4})));

    for (const std::shared_ptr<air::Stmt>& s :
            {cfg->getEntry(), s1, s2, s3, s4, s5, s6, s7, cfg->getExit()}) {
        CHECK(result->getResult(s)->equalsTo(result->getOutFact(s)));
    }

    al::World::getLogger().Success("Finish testing reaching definition example loop with while ...");

}

TEST_SUITE_END();

