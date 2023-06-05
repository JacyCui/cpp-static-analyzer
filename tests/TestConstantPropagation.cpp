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
    std::shared_ptr<air::IR> dummy, typeCast, ifElse, binaryOp, loop, incDec, array, call;
    std::unique_ptr<df::ConstantPropagation> cp;
    
public:
    ConstPropagationTestFixture() {
        al::World::initialize("resources/dataflow/ConstPropagation");
        const al::World& world = al::World::get();
        dummy = world.getMethodBySignature("int ConstPropagation::dummy()")->getIR();
        typeCast = world.getMethodBySignature("void ConstPropagation::typeCast()")->getIR();
        ifElse = world.getMethodBySignature("int ConstPropagation::ifElse(int)")->getIR();
        binaryOp = world.getMethodBySignature("int ConstPropagation::binaryOp(int)")->getIR();
        loop = world.getMethodBySignature("int ConstPropagation::loop()")->getIR();
        incDec = world.getMethodBySignature("void ConstPropagation::incDec()")->getIR();
        array = world.getMethodBySignature("int ConstPropagation::array()")->getIR();
        call = world.getMethodBySignature("int ConstPropagation::call()")->getIR();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig;
        analysisConfig = std::make_unique<cf::DefaultAnalysisConfig>("constant propagation analysis");
        cp = std::make_unique<df::ConstantPropagation>(analysisConfig);

        CHECK(analysisConfig == nullptr);
    }
};


TEST_SUITE_BEGIN("testConstantPropagation");

TEST_CASE_FIXTURE(ConstPropagationTestFixture, "testConstPropagationCaseDummy"
    * doctest::description("testing constant propagation example dummy")) {

    al::World::getLogger().Progress("Testing constant propagation example dummy ...");

    std::shared_ptr<graph::CFG> cfg = dummy->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : dummy->getStmts()) {
        al::World::getLogger().Debug(s->str());
        stmtMap.emplace(s->str(), s);
    }

    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    for (const std::shared_ptr<air::Var>& v : dummy->getVars()) {
        varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("int x;");
    std::shared_ptr<air::Stmt> s2 = stmtMap.at("int y;");
    std::shared_ptr<air::Stmt> s3 = stmtMap.at("y = x = 1");
    std::shared_ptr<air::Stmt> s4 = stmtMap.at("return y");

    std::shared_ptr<air::Var> x = varMap.at("x");
    std::shared_ptr<air::Var> y = varMap.at("y");

    std::shared_ptr<dfact::DataflowResult<df::CPFact>> result = cp->analyze(dummy);

    CHECK(result->getOutFact(s1)->get(x)->isUndef());
    CHECK(result->getOutFact(s2)->get(x)->isUndef());
    auto s3_x = result->getOutFact(s3)->get(x);
    CHECK(s3_x->isConstant());
    CHECK_EQ(s3_x->getConstantValue(), 1);
    auto s3_y = result->getOutFact(s4)->get(y);
    CHECK(s3_y->isConstant());
    CHECK_EQ(s3_y->getConstantValue(), 1);

    al::World::getLogger().Success("Finish testing constant propagation example dummy ...");

}

TEST_CASE_FIXTURE(ConstPropagationTestFixture, "testConstPropagationTypeCase"
    * doctest::description("testing constant propagation example type-cast")) {

    al::World::getLogger().Progress("Testing constant propagation example type-cast ...");

    std::shared_ptr<graph::CFG> cfg = typeCast->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : typeCast->getStmts()) {
        al::World::getLogger().Debug(s->str());
        stmtMap.emplace(s->str(), s);
    }

    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    for (const std::shared_ptr<air::Var>& v : typeCast->getVars()) {
        varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> exit = cfg->getExit();

    std::shared_ptr<air::Var> a = varMap.at("a");
    std::shared_ptr<air::Var> x = varMap.at("x");
    std::shared_ptr<air::Var> y = varMap.at("y");
    std::shared_ptr<air::Var> z = varMap.at("z");

    std::shared_ptr<dfact::DataflowResult<df::CPFact>> result = cp->analyze(typeCast);
    auto fact = result->getInFact(exit);

    CHECK(fact->get(a)->isConstant());
    CHECK_EQ(fact->get(a)->getConstantValue(), (char)'a');
    CHECK(fact->get(x)->isConstant());
    CHECK_EQ(fact->get(x)->getConstantValue(), (int)(char)10000000);
    CHECK(fact->get(y)->isConstant());
    CHECK_EQ(fact->get(y)->getConstantValue(), (long long)1000000000000000000);
    CHECK(fact->get(z)->isConstant());
    CHECK_EQ(fact->get(z)->getConstantValue(), (unsigned long long)(unsigned int)-1);

    al::World::getLogger().Success("Finish testing constant propagation example type-cast ...");

}

TEST_CASE_FIXTURE(ConstPropagationTestFixture, "testConstPropagationCaseIfElse"
    * doctest::description("testing constant propagation example if-else")) {

    al::World::getLogger().Progress("Testing constant propagation example if-else ...");

    std::shared_ptr<graph::CFG> cfg = ifElse->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ifElse->getStmts()) {
       al::World::getLogger().Debug(s->str());
       stmtMap.emplace(s->str(), s);
    }

    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    for (const std::shared_ptr<air::Var>& v : ifElse->getVars()) {
       varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> exit = cfg->getExit();

    std::shared_ptr<air::Var> n = varMap.at("n");
    std::shared_ptr<air::Var> x = varMap.at("x");
    std::shared_ptr<air::Var> y = varMap.at("y");
    std::shared_ptr<air::Var> z = varMap.at("z");
    std::shared_ptr<air::Var> a = varMap.at("a");
    std::shared_ptr<air::Var> u = varMap.at("u");
    std::shared_ptr<air::Var> v = varMap.at("v");

    std::shared_ptr<dfact::DataflowResult<df::CPFact>> result = cp->analyze(ifElse);
    auto fact = result->getInFact(exit);

    CHECK(fact->get(n)->isNAC());
    CHECK(fact->get(x)->isNAC());
    CHECK(fact->get(y)->isConstant());
    CHECK_EQ(fact->get(y)->getConstantValue(), 3);
    CHECK(fact->get(z)->isNAC());
    CHECK(fact->get(a)->isUndef());
    CHECK(fact->get(u)->isConstant());
    CHECK_EQ(fact->get(u)->getConstantValue(), 2);
    CHECK(fact->get(v)->isNAC());

    al::World::getLogger().Success("Finish constant propagation example if-else ...");

}

TEST_CASE_FIXTURE(ConstPropagationTestFixture, "testConstPropagationCaseBinaryOp"
    * doctest::description("testing constant propagation example binary-op")) {

    al::World::getLogger().Progress("Testing constant propagation example binary-op ...");

    std::shared_ptr<graph::CFG> cfg = binaryOp->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : binaryOp->getStmts()) {
       al::World::getLogger().Debug(s->str());
       stmtMap.emplace(s->str(), s);
    }

    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    for (const std::shared_ptr<air::Var>& v : binaryOp->getVars()) {
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

    std::shared_ptr<dfact::DataflowResult<df::CPFact>> result = cp->analyze(binaryOp);
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

TEST_CASE_FIXTURE(ConstPropagationTestFixture, "testConstPropagationCaseLoop"
    * doctest::description("testing constant propagation example loop")) {
    
    al::World::getLogger().Progress("Testing constant propagation example loop ...");

    std::shared_ptr<graph::CFG> cfg = loop->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : loop->getStmts()) {
       al::World::getLogger().Debug(s->str());
       stmtMap.emplace(s->str(), s);
    }

    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    for (const std::shared_ptr<air::Var>& v : loop->getVars()) {
       varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> exit = cfg->getExit();

    std::shared_ptr<air::Var> a = varMap.at("a");
    std::shared_ptr<air::Var> c = varMap.at("c");

    std::shared_ptr<dfact::DataflowResult<df::CPFact>> result = cp->analyze(loop);

    auto fact = result->getInFact(exit);
    CHECK(fact->get(a)->isNAC());
    CHECK(fact->get(c)->isConstant());
    CHECK_EQ(fact->get(c)->getConstantValue(), 2);

    al::World::getLogger().Success("Finish constant propagation example loop ...");
}

TEST_CASE_FIXTURE(ConstPropagationTestFixture, "testConstPropagationIncDec"
    * doctest::description("testing constant propagation example inc-dec")) {

    al::World::getLogger().Progress("Testing constant propagation example inc-dec ...");

    std::shared_ptr<graph::CFG> cfg = incDec->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : incDec->getStmts()) {
    al::World::getLogger().Debug(s->str());
    stmtMap.emplace(s->str(), s);
    }

    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    for (const std::shared_ptr<air::Var>& v : incDec->getVars()) {
    varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<air::Stmt> exit = cfg->getExit();

    std::shared_ptr<air::Var> a = varMap.at("a");
    std::shared_ptr<air::Var> b = varMap.at("b");
    std::shared_ptr<air::Var> c = varMap.at("c");
    std::shared_ptr<air::Var> d = varMap.at("d");

    std::shared_ptr<dfact::DataflowResult<df::CPFact>> result = cp->analyze(incDec);

    auto fact = result->getInFact(exit);
    CHECK(fact->get(a)->isConstant());
    CHECK_EQ(fact->get(a)->getConstantValue(), 1);
    CHECK(fact->get(b)->isConstant());
    CHECK_EQ(fact->get(b)->getConstantValue(), 3);
    CHECK(fact->get(c)->isConstant());
    CHECK_EQ(fact->get(c)->getConstantValue(), -1);
    CHECK(fact->get(d)->isConstant());
    CHECK_EQ(fact->get(d)->getConstantValue(), -3);

    al::World::getLogger().Success("Finish constant propagation example inc-dec ...");
}

TEST_CASE_FIXTURE(ConstPropagationTestFixture, "testConstPropagationArray"
    * doctest::description("testing constant propagation example array")) {

    al::World::getLogger().Progress("Testing constant propagation example array ...");

    std::shared_ptr<graph::CFG> cfg = array->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : array->getStmts()) {
    al::World::getLogger().Debug(s->str());
    stmtMap.emplace(s->str(), s);
    }

    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    for (const std::shared_ptr<air::Var>& v : array->getVars()) {
    varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<dfact::DataflowResult<df::CPFact>> df_result = cp->analyze(array);
    std::shared_ptr<df::CPResult> result = std::dynamic_pointer_cast<df::CPResult>(df_result);

    std::shared_ptr<air::Stmt> stmt = stmtMap.at("a[i][j]");
    /*
    `-ImplicitCastExpr 0x14580cda8 <col:16, col:22> 'int' <LValueToRValue>
    `-ArraySubscriptExpr 0x14580cd88 <col:16, col:22> 'int' lvalue
    |-ImplicitCastExpr1 0x14580cd58 <col:16, col:19> 'int *' <ArrayToPointerDecay>
    | `-ArraySubscriptExpr1 0x14580cce8 <col:16, col:19> 'int[2]' lvalue
    |   |-ImplicitCastExpr2 0x14580ccb8 <col:16> 'int (*)[2]' <ArrayToPointerDecay>
    |   | `-DeclRefExpr 0x14580cc50 <col:16> 'int[2][2]' lvalue Var 0x14580ca60 'a' 'int[2][2]'
    |   `-ImplicitCastExpr_i 0x14580ccd0 <col:18> 'int' <LValueToRValue>
    |     `-DeclRefExpr 0x14580cc70 <col:18> 'int' lvalue Var 0x14580caf8 'i' 'int'
    `-ImplicitCastExpr 0x14580cd70 <col:21> 'int' <LValueToRValue>
     `-DeclRefExpr 0x14580cd08 <col:21> 'int' lvalue Var 0x14580cb98 'j' 'int'
    */

    const clang::Stmt *clangStmt = stmt->getClangStmt();
    auto *implicitCastExpr = clang::dyn_cast<clang::ImplicitCastExpr>(clangStmt);
    auto *arraySubscriptExpr = clang::dyn_cast<clang::ArraySubscriptExpr>(implicitCastExpr->getSubExpr());
    // check type: int[2][2]
    auto *implicitCastExpr1 = clang::dyn_cast<clang::ImplicitCastExpr>(arraySubscriptExpr->getBase());
    auto *arraySubscriptExpr1 = clang::dyn_cast<clang::ArraySubscriptExpr>(implicitCastExpr1->getSubExpr());
    auto *implicitCastExpr2 = clang::dyn_cast<clang::ImplicitCastExpr>(arraySubscriptExpr1->getBase());
    auto *declRefExpr = clang::dyn_cast<clang::DeclRefExpr>(implicitCastExpr2->getSubExpr());
    auto *arrayType2 = clang::dyn_cast<clang::ConstantArrayType>(declRefExpr->getType());
    CHECK(arrayType2 != nullptr);
    CHECK_EQ(arrayType2->getSize(), 2);
    auto *implicitCastExpr_i = clang::dyn_cast<clang::ImplicitCastExpr>(arraySubscriptExpr1->getIdx());
    auto i_value = result->getExprValue(implicitCastExpr_i);
    CHECK(i_value != nullptr);
    CHECK(i_value->isConstant());
    CHECK_EQ(i_value->getConstantValue(), 2);
    // check type: int[2]
    auto *arrayType1 = clang::dyn_cast<clang::ConstantArrayType>(arraySubscriptExpr1->getType());
    CHECK(arrayType1 != nullptr);
    CHECK_EQ(arrayType1->getSize(), 2);
    auto *implicitCastExpr_j = clang::dyn_cast<clang::ImplicitCastExpr>(arraySubscriptExpr->getIdx());
    auto j_value = result->getExprValue(implicitCastExpr_j);
    CHECK(j_value != nullptr);
    CHECK(j_value->isConstant());
    CHECK_EQ(j_value->getConstantValue(), 3);

    al::World::getLogger().Success("Finish constant propagation example array ...");

}

TEST_CASE_FIXTURE(ConstPropagationTestFixture, "testConstPropagationCall"
    * doctest::description("testing constant propagation example call")) {

    al::World::getLogger().Progress("Testing constant propagation example call ...");

    std::shared_ptr<graph::CFG> cfg = call->getCFG();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : call->getStmts()) {
    al::World::getLogger().Debug(s->str());
    stmtMap.emplace(s->str(), s);
    }

    std::unordered_map<std::string, std::shared_ptr<air::Var>> varMap;
    for (const std::shared_ptr<air::Var>& v : call->getVars()) {
    varMap.emplace(v->getName(), v);
    }

    std::shared_ptr<dfact::DataflowResult<df::CPFact>> df_result = cp->analyze(call);
    std::shared_ptr<df::CPResult> result = std::dynamic_pointer_cast<df::CPResult>(df_result);

    std::shared_ptr<air::Stmt> stmt = stmtMap.at("foo(1 / zero)");
    auto *clangStmt = stmt->getClangStmt();
    auto *callExpr = clang::dyn_cast<clang::CallExpr>(clangStmt);
    auto *arg0 = callExpr->getArg(0);

    auto arg0_value = result->getExprValue(arg0);
    CHECK(arg0_value != nullptr);
    CHECK(arg0_value->isUndef());

    al::World::getLogger().Success("Finish constant propagation example call ...");
}

TEST_SUITE_END();

