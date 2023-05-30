#include "doctest.h"

#include "World.h"
#include "analysis/dataflow/ConstantPropagation.h"

#include <iostream>

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace graph = al::analysis::graph;
namespace df = al::analysis::dataflow;
namespace dfact = al::analysis::dataflow::fact;

class ConstPropagationTestFixture {
protected:
    std::shared_ptr<air::IR> ir1, ir2, ir3;
    std::unique_ptr<df::ConstantPropagation> cp;
public:
    ConstPropagationTestFixture() {
        al::World::initialize("resources/dataflow/constPropagation");
        const al::World& world = al::World::get();
        ir1 = world.getMethodBySignature("int ConstPropagation::dummy()")->getIR();
        ir2 = world.getMethodBySignature("int ConstPropagation::ifElse(int)")->getIR();
        ir3 = world.getMethodBySignature("int ConstPropagation::binaryOp(int)")->getIR();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig
                = std::make_unique<cf::DefaultAnalysisConfig>("constant propagation analysis");
        cp = std::make_unique<df::ConstantPropagation>(analysisConfig);

        CHECK_FALSE(analysisConfig);
    }
};


TEST_SUITE_BEGIN("testConstantPropagation");

TEST_CASE_FIXTURE(ConstPropagationTestFixture, "testConstPropagationCaseDummy"
    * doctest::description("testing constant propagation example dummy")) {

    al::World::getLogger().Progress("Testing constant propagation example dummy ...");

    std::shared_ptr<graph::CFG> cfg = ir1->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir1->getStmts()) {
        al::World::getLogger().Debug(s->str());
        stmtMap.emplace(s->str(), s);
    }

    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    for (const std::shared_ptr<air::Var>& v : ir1->getVars()) {
        varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("int x;");
    std::shared_ptr<air::Stmt> s2 = stmtMap.at("int y;");
    std::shared_ptr<air::Stmt> s3 = stmtMap.at("x = 1");
    std::shared_ptr<air::Stmt> s4 = stmtMap.at("y = x");
    std::shared_ptr<air::Stmt> s5 = stmtMap.at("return y");

    std::shared_ptr<air::Var> x = varMap.at("x");
    std::shared_ptr<air::Var> y = varMap.at("y");

    std::shared_ptr<dfact::DataflowResult<df::CPFact>> result = cp->analyze(ir1);

    CHECK(result->getOutFact(s1)->get(x)->isUndef());
    CHECK(result->getOutFact(s2)->get(x)->isUndef());
    auto s3_x = result->getOutFact(s3)->get(x);
    CHECK(s3_x->isConstant());
    CHECK_EQ(s3_x->getConstantValue(), 1);
    auto s4_x = result->getOutFact(s4)->get(x);
    CHECK(s4_x->isConstant());
    CHECK_EQ(s4_x->getConstantValue(), 1);
    auto s4_y = result->getOutFact(s4)->get(y);
    CHECK(s4_y->isConstant());
    CHECK_EQ(s4_y->getConstantValue(), 1);

    al::World::getLogger().Success("Finish testing constant propagation example dummy ...");

}

TEST_CASE_FIXTURE(ConstPropagationTestFixture, "testConstPropagationCaseIfElse"
    * doctest::description("testing constant propagation example if-else")) {

    al::World::getLogger().Progress("Testing constant propagation example if-else ...");

    std::shared_ptr<graph::CFG> cfg = ir2->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir2->getStmts()) {
       al::World::getLogger().Debug(s->str());
       stmtMap.emplace(s->str(), s);
    }

    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    for (const std::shared_ptr<air::Var>& v : ir2->getVars()) {
       varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> exit = cfg->getExit();

    std::shared_ptr<air::Var> n = varMap.at("n");
    std::shared_ptr<air::Var> x = varMap.at("x");
    std::shared_ptr<air::Var> y = varMap.at("y");
    std::shared_ptr<air::Var> z = varMap.at("z");
    std::shared_ptr<air::Var> a = varMap.at("a");

    std::shared_ptr<dfact::DataflowResult<df::CPFact>> result = cp->analyze(ir2);
    auto fact = result->getInFact(exit);

    CHECK(fact->get(n)->isNAC());
    CHECK(fact->get(x)->isNAC());
    CHECK(fact->get(y)->isConstant());
    CHECK_EQ(fact->get(y)->getConstantValue(), 3);
    CHECK(fact->get(z)->isNAC());
    CHECK(fact->get(a)->isUndef());

    al::World::getLogger().Success("Finish constant propagation example if-else ...");

}

TEST_CASE_FIXTURE(ConstPropagationTestFixture, "testConstPropagationCaseBinaryOp"
    * doctest::description("testing constant propagation example binary-op")) {

    al::World::getLogger().Progress("Testing constant propagation example binary-op ...");

    std::shared_ptr<graph::CFG> cfg = ir3->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir3->getStmts()) {
       al::World::getLogger().Debug(s->str());
       stmtMap.emplace(s->str(), s);
    }

    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    for (const std::shared_ptr<air::Var>& v : ir3->getVars()) {
       varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> exit = cfg->getExit();

    std::shared_ptr<air::Var> n = varMap.at("n");
    std::shared_ptr<air::Var> x = varMap.at("x");
    std::shared_ptr<air::Var> y = varMap.at("y");
    std::shared_ptr<air::Var> neg = varMap.at("neg");
    std::shared_ptr<air::Var> add = varMap.at("add");
    std::shared_ptr<air::Var> sub = varMap.at("sub");
    std::shared_ptr<air::Var> mul = varMap.at("mul");
    std::shared_ptr<air::Var> div = varMap.at("div");
    std::shared_ptr<air::Var> mod = varMap.at("mod");
    std::shared_ptr<air::Var> And = varMap.at("And");
    std::shared_ptr<air::Var> Or = varMap.at("Or");
    std::shared_ptr<air::Var> Xor = varMap.at("Xor");
    std::shared_ptr<air::Var> LShift = varMap.at("LShift");
    std::shared_ptr<air::Var> RShift = varMap.at("RShift");
    std::shared_ptr<air::Var> zero = varMap.at("zero");


    std::shared_ptr<dfact::DataflowResult<df::CPFact>> result = cp->analyze(ir3);
    CHECK(result->getOutFact(cfg->getEntry())->get(n)->isNAC());

    auto fact = result->getInFact(exit);
    CHECK(fact->get(x)->isConstant());
    CHECK_EQ(fact->get(x)->getConstantValue(), 15);
    CHECK(fact->get(y)->isConstant());
    CHECK_EQ(fact->get(y)->getConstantValue(), 2);
    CHECK(fact->get(neg)->isConstant());
    CHECK_EQ(fact->get(neg)->getConstantValue(), -15);
    CHECK(fact->get(add)->isConstant());
    CHECK_EQ(fact->get(add)->getConstantValue(), 15 + 2);
    CHECK(fact->get(sub)->isConstant());
    CHECK_EQ(fact->get(sub)->getConstantValue(), 15 - 2);
    CHECK(fact->get(mul)->isConstant());
    CHECK_EQ(fact->get(mul)->getConstantValue(), 15 * 2);
    CHECK(fact->get(div)->isConstant());
    CHECK_EQ(fact->get(div)->getConstantValue(), 15 / 2);
    CHECK(fact->get(mod)->isConstant());
    CHECK_EQ(fact->get(mod)->getConstantValue(), 15 % 2);
    CHECK(fact->get(And)->isConstant());
    CHECK_EQ(fact->get(And)->getConstantValue(), 15 & 2);
    CHECK(fact->get(Or)->isConstant());
    CHECK_EQ(fact->get(Or)->getConstantValue(), 15 | 2);
    CHECK(fact->get(Xor)->isConstant());
    CHECK_EQ(fact->get(Xor)->getConstantValue(), 15 ^ 2);
    CHECK(fact->get(LShift)->isConstant());
    CHECK_EQ(fact->get(LShift)->getConstantValue(), 15 << 2);
    CHECK(fact->get(RShift)->isConstant());
    CHECK_EQ(fact->get(RShift)->getConstantValue(), 15 >> 2);
    CHECK(fact->get(zero)->isConstant());
    CHECK_EQ(fact->get(zero)->getConstantValue(), 0);
    CHECK(fact->get(n)->isUndef());

    al::World::getLogger().Success("Finish constant propagation example binary-op ...");

}

TEST_SUITE_END();

