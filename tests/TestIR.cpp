#include "doctest.h"

#include "World.h"
#include "ir/IR.h"

namespace al = analyzer;
namespace air = al::ir;
namespace graph = al::analysis::graph;

class IRTestFixture {
protected:
    std::shared_ptr<air::IR> ir1, ir2, ir3, ir4, ir5;
public:
    IRTestFixture() {
        al::World::initialize("resources/example02");
        const al::World& world = al::World::get();
        ir1 = world.getMethodBySignature("int main(int, char **)")->getIR();
        ir2 = world.getMethodBySignature("int test1(int, int)")->getIR();
        ir3 = world.getMethodBySignature("int fib(int)")->getIR();
        ir4 = world.getMethodBySignature("int factor(int)")->getIR();
        ir5 = world.getMethodBySignature("void test2(int, int, int, int, int)")->getIR();
    }
};

TEST_SUITE_BEGIN("testIR");

TEST_CASE_FIXTURE(IRTestFixture, "testIRBuilding"
    * doctest::description("testing successful ir building")) {

    al::World::getLogger().Success("Finish testing successful ir building ...");

}

TEST_CASE_FIXTURE(IRTestFixture, "testGetParams"
    * doctest::description("testing get method ir parameters")) {

    al::World::getLogger().Progress("Testing get method ir parameters ...");

    std::vector<std::shared_ptr<air::Var>> params1 = ir1->getParams();
    CHECK_EQ(params1.size(), 2);
    CHECK_EQ(params1.at(0)->getType()->getName(), "int");
    CHECK_EQ(params1.at(0)->getName(), "argc");
    CHECK_EQ(params1.at(1)->getType()->getName(), "char **");
    CHECK_EQ(params1.at(1)->getName(), "argv");

    std::vector<std::shared_ptr<air::Var>> params2 = ir2->getParams();
    CHECK_EQ(params2.size(), 2);
    CHECK_EQ(params2.at(0)->getType()->getName(), "int");
    CHECK_EQ(params2.at(0)->getName(), "a");
    CHECK_EQ(params2.at(1)->getType()->getName(), "int");
    CHECK_EQ(params2.at(1)->getName(), "b");

    std::vector<std::shared_ptr<air::Var>> params3 = ir3->getParams();
    CHECK_EQ(params3.size(), 1);
    CHECK_EQ(params3.at(0)->getType()->getName(), "int");
    CHECK_EQ(params3.at(0)->getName(), "i");

    std::vector<std::shared_ptr<air::Var>> params4 = ir4->getParams();
    CHECK_EQ(params4.size(), 1);
    CHECK_EQ(params4.at(0)->getType()->getName(), "int");
    CHECK_EQ(params4.at(0)->getName(), "n");

    al::World::getLogger().Success("Finish testing get method ir parameters ...");

}

TEST_CASE_FIXTURE(IRTestFixture, "testGetVars"
    * doctest::description("testing get method variables")) {

    al::World::getLogger().Progress("Testing get method variables ...");
    std::vector<std::shared_ptr<air::Var>>&& vars1 = ir1->getVars();
    std::unordered_map<std::string, std::string> output1;
    for (const std::shared_ptr<air::Var>& v : vars1) {
        output1.emplace(v->getName(), v->getType()->getName());
    }
    CHECK_EQ(output1, std::unordered_map<std::string, std::string>{
        {"a", "int"}, {"argc", "int"}, {"argv", "char **"}, {"b", "int"},
        {"c", "int"}, {"d", "int"}, {"e", "int"}
    });

    std::vector<std::shared_ptr<air::Var>>&& vars2 = ir2->getVars();
    std::unordered_map<std::string, std::string> output2;
    for (const std::shared_ptr<air::Var>& v : vars2) {
        output2.emplace(v->getName(), v->getType()->getName());
    }
    CHECK_EQ(output2, std::unordered_map<std::string, std::string>{
        {"a", "int"},  {"b", "int"}, {"c", "int"}, {"d", "int"}
    });

    std::vector<std::shared_ptr<air::Var>>&& vars3 = ir3->getVars();
    std::unordered_map<std::string, std::string> output3;
    for (const std::shared_ptr<air::Var>& v : vars3) {
        output3.emplace(v->getName(), v->getType()->getName());
    }
    CHECK_EQ(output3, std::unordered_map<std::string, std::string>{
            {"a", "int"},  {"b", "int"}, {"i", "int"}, {"tmp", "int"}
    });

    std::vector<std::shared_ptr<air::Var>>&& vars4 = ir4->getVars();
    std::unordered_map<std::string, std::string> output4;
    for (const std::shared_ptr<air::Var>& v : vars4) {
        output4.emplace(v->getName(), v->getType()->getName());
    }
    CHECK_EQ(output4, std::unordered_map<std::string, std::string>{
            {"i", "int"},  {"n", "int"}, {"result", "int"}
    });

    al::World::getLogger().Success("Finish testing get method variables ...");

}

TEST_CASE_FIXTURE(IRTestFixture, "testGetStmts"
    * doctest::description("testing get method statements")) {

    al::World::getLogger().Progress("Testing get method statements ...");

    std::vector<std::shared_ptr<air::Stmt>>&& stmts1 = ir1->getStmts();
    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : stmts1) {
        stmtMap.emplace(s->str(), s);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("int a = 3;");
    CHECK_EQ(s1->getDefs().size(), 1);
    CHECK_EQ(s1->getUses().size(), 0);
    CHECK_EQ((*s1->getDefs().begin())->getName(), "a");
    CHECK_EQ((*s1->getDefs().begin())->getType()->getName(), "int");
    CHECK_EQ(s1->getStartLine(), 13);
    CHECK_EQ(s1->getStartColumn(), 5);
    CHECK_EQ(s1->getEndLine(), 13);
    CHECK_EQ(s1->getEndColumn(), 14);

    std::shared_ptr<air::Stmt> s2 = stmtMap.at("int b;");
    CHECK_EQ(s2->getDefs().size(), 0);
    CHECK_EQ(s2->getUses().size(), 0);
    CHECK_EQ(s2->getStartLine(), 14);
    CHECK_EQ(s2->getStartColumn(), 5);
    CHECK_EQ(s2->getEndLine(), 14);
    CHECK_EQ(s2->getEndColumn(), 10);

    std::shared_ptr<air::Stmt> s3 = stmtMap.at("int c;");
    CHECK_EQ(s3->getDefs().size(), 0);
    CHECK_EQ(s3->getUses().size(), 0);
    CHECK_EQ(s3->getStartLine(), 15);
    CHECK_EQ(s3->getStartColumn(), 5);
    CHECK_EQ(s3->getEndLine(), 15);
    CHECK_EQ(s3->getEndColumn(), 10);

    std::shared_ptr<air::Stmt> s4 = stmtMap.at("a = 1");
    CHECK_EQ(s4->getDefs().size(), 1);
    CHECK_EQ(s4->getUses().size(), 0);
    CHECK_EQ((*s4->getDefs().begin())->getName(), "a");
    CHECK_EQ((*s4->getDefs().begin())->getType()->getName(), "int");
    CHECK_EQ(s4->getStartLine(), 16);
    CHECK_EQ(s4->getStartColumn(), 5);
    CHECK_EQ(s4->getEndLine(), 16);
    CHECK_EQ(s4->getEndColumn(), 9);

    std::shared_ptr<air::Stmt> s5 = stmtMap.at("a > 1");
    CHECK_EQ(s5->getDefs().size(), 0);
    CHECK_EQ(s5->getUses().size(), 1);
    CHECK_EQ((*s5->getUses().begin())->getName(), "a");
    CHECK_EQ((*s5->getUses().begin())->getType()->getName(), "int");
    CHECK_EQ(s5->getStartLine(), 17);
    CHECK_EQ(s5->getStartColumn(), 9);
    CHECK_EQ(s5->getEndLine(), 17);
    CHECK_EQ(s5->getEndColumn(), 13);

    std::shared_ptr<air::Stmt> s6 = stmtMap.at("b = 2");
    CHECK_EQ(s6->getDefs().size(), 1);
    CHECK_EQ(s6->getUses().size(), 0);
    CHECK_EQ((*s6->getDefs().begin())->getName(), "b");
    CHECK_EQ((*s6->getDefs().begin())->getType()->getName(), "int");
    CHECK_EQ(s6->getStartLine(), 18);
    CHECK_EQ(s6->getStartColumn(), 9);
    CHECK_EQ(s6->getEndLine(), 18);
    CHECK_EQ(s6->getEndColumn(), 13);

    std::shared_ptr<air::Stmt> s7 = stmtMap.at("b = 3");
    CHECK_EQ(s7->getDefs().size(), 1);
    CHECK_EQ(s7->getUses().size(), 0);
    CHECK_EQ((*s7->getDefs().begin())->getName(), "b");
    CHECK_EQ((*s7->getDefs().begin())->getType()->getName(), "int");
    CHECK_EQ(s7->getStartLine(), 20);
    CHECK_EQ(s7->getStartColumn(), 9);
    CHECK_EQ(s7->getEndLine(), 20);
    CHECK_EQ(s7->getEndColumn(), 13);

    std::shared_ptr<air::Stmt> s8 = stmtMap.at("c = b + a");
    CHECK_EQ(s8->getDefs().size(), 1);
    CHECK_EQ(s8->getUses().size(), 2);
    CHECK_EQ((*s8->getDefs().begin())->getName(), "c");
    CHECK_EQ((*s8->getDefs().begin())->getType()->getName(), "int");
    std::unordered_map<std::string, std::string> output8;
    for (const std::shared_ptr<air::Var>& use : s8->getUses()) {
        output8.emplace(use->getName(), use->getType()->getName());
    }
    CHECK_EQ(output8, std::unordered_map<std::string, std::string>{
            {"a", "int"}, {"b", "int"}
    });
    CHECK_EQ(s8->getStartLine(), 22);
    CHECK_EQ(s8->getStartColumn(), 5);
    CHECK_EQ(s8->getEndLine(), 22);
    CHECK_EQ(s8->getEndColumn(), 13);

    std::shared_ptr<air::Stmt> s9 = stmtMap.at("e = 1");
    CHECK_EQ(s9->getDefs().size(), 1);
    CHECK_EQ(s9->getUses().size(), 0);
    CHECK_EQ((*s9->getDefs().begin())->getName(), "e");
    CHECK_EQ((*s9->getDefs().begin())->getType()->getName(), "int");
    CHECK_EQ(s9->getStartLine(), 23);
    CHECK_EQ(s9->getStartColumn(), 9);
    CHECK_EQ(s9->getEndLine(), 23);
    CHECK_EQ(s9->getEndColumn(), 13);

    std::shared_ptr<air::Stmt> s10 = stmtMap.at("d");
    CHECK_EQ(s10->getDefs().size(), 0);
    CHECK_EQ(s10->getUses().size(), 0);
    CHECK_EQ(s10->getStartLine(), 23);
    CHECK_EQ(s10->getStartColumn(), 16);
    CHECK_EQ(s10->getEndLine(), 23);
    CHECK_EQ(s10->getEndColumn(), 16);

    std::shared_ptr<air::Stmt> s11 = stmtMap.at("c");
    CHECK_EQ(s11->getDefs().size(), 0);
    CHECK_EQ(s11->getUses().size(), 1);
    CHECK_EQ((*s11->getUses().begin())->getName(), "c");
    CHECK_EQ((*s11->getUses().begin())->getType()->getName(), "int");
    CHECK_EQ(s11->getStartLine(), 24);
    CHECK_EQ(s11->getStartColumn(), 12);
    CHECK_EQ(s11->getEndLine(), 24);
    CHECK_EQ(s11->getEndColumn(), 12);

    std::shared_ptr<air::Stmt> s12 = stmtMap.at("return c");
    CHECK_EQ(s12->getDefs().size(), 0);
    CHECK_EQ(s12->getUses().size(), 1);
    CHECK_EQ((*s12->getUses().begin())->getName(), "c");
    CHECK_EQ((*s12->getUses().begin())->getType()->getName(), "int");
    CHECK_EQ(s12->getStartLine(), 24);
    CHECK_EQ(s12->getStartColumn(), 5);
    CHECK_EQ(s12->getEndLine(), 24);
    CHECK_EQ(s12->getEndColumn(), 12);

    al::World::getLogger().Success("Finish testing get method statements ...");

}

TEST_CASE_FIXTURE(IRTestFixture, "testGetCFG"
    * doctest::description("testing get method cfg")) {

    al::World::getLogger().Progress("Testing get method cfg ...");

    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir1->getStmts()) {
        stmtMap.emplace(s->str(), s);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("int a = 3;");
    std::shared_ptr<air::Stmt> s2 = stmtMap.at("int b;");
    std::shared_ptr<air::Stmt> s3 = stmtMap.at("int c;");
    std::shared_ptr<air::Stmt> s4 = stmtMap.at("a = 1");
    std::shared_ptr<air::Stmt> s5 = stmtMap.at("a > 1");
    std::shared_ptr<air::Stmt> s6 = stmtMap.at("b = 2");
    std::shared_ptr<air::Stmt> s7 = stmtMap.at("b = 3");
    std::shared_ptr<air::Stmt> s8 = stmtMap.at("c = b + a");
    std::shared_ptr<air::Stmt> s9 = stmtMap.at("e = 1");
    std::shared_ptr<air::Stmt> s10 = stmtMap.at("d");
    std::shared_ptr<air::Stmt> s11 = stmtMap.at("c");
    std::shared_ptr<air::Stmt> s12 = stmtMap.at("return c");

    std::shared_ptr<graph::CFG> cfg = ir1->getCFG();

    CHECK_EQ(cfg->getEdgeNum(), 14);
    CHECK(cfg->hasEdge(cfg->getEntry(), s1));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(cfg->getEntry())) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::ENTRY_EDGE);
    }

    CHECK(cfg->hasEdge(s1, s2));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(s1)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s2, s3));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(s2)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s3, s4));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(s3)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s4, s5));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(s4)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s5, s6));
    CHECK(cfg->hasEdge(s5, s7));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(s5)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::JUMP_EDGE);
    }

    CHECK(cfg->hasEdge(s6, s8));
    CHECK(cfg->hasEdge(s7, s8));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getInEdgesOf(s8)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::JUMP_EDGE);
    }

    CHECK(cfg->hasEdge(s8, s9));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getInEdgesOf(s9)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s9, s10));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getInEdgesOf(s10)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s10, s11));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getInEdgesOf(s11)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s11, s12));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getInEdgesOf(s12)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s12, cfg->getExit()));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getInEdgesOf(cfg->getExit())) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::EXIT_EDGE);
    }

    al::World::getLogger().Success("Testing get method cfg ...");

}

TEST_CASE_FIXTURE(IRTestFixture, "testGetCFG2"
    * doctest::description("testing get another method cfg")) {

    al::World::getLogger().Progress("Testing get another method cfg ...");

    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir3->getStmts()) {
        stmtMap.emplace(s->str(), s);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("a = 0");
    std::shared_ptr<air::Stmt> s2 = stmtMap.at("b = 1");
    std::shared_ptr<air::Stmt> s3 = stmtMap.at("i > 0");
    std::shared_ptr<air::Stmt> s4 = stmtMap.at("int tmp = b;");
    std::shared_ptr<air::Stmt> s5 = stmtMap.at("b = a + b");
    std::shared_ptr<air::Stmt> s6 = stmtMap.at("a = tmp");
    std::shared_ptr<air::Stmt> s7 = stmtMap.at("i--");
    std::shared_ptr<air::Stmt> s8 = stmtMap.at("nop");
    std::shared_ptr<air::Stmt> s9 = stmtMap.at("a");
    std::shared_ptr<air::Stmt> s10 = stmtMap.at("return a");

    std::shared_ptr<graph::CFG> cfg = ir3->getCFG();

    CHECK_EQ(cfg->getEdgeNum(), 12);

    CHECK(cfg->hasEdge(cfg->getEntry(), s1));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(cfg->getEntry())) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::ENTRY_EDGE);
    }

    CHECK(cfg->hasEdge(s1, s2));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(s1)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s2, s3));
    CHECK(cfg->hasEdge(s8, s3));
    CHECK(cfg->hasEdge(s3, s4));
    CHECK(cfg->hasEdge(s3, s9));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getInEdgesOf(s3)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::JUMP_EDGE);
    }
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(s3)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::JUMP_EDGE);
    }

    CHECK(cfg->hasEdge(s4, s5));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(s4)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s5, s6));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(s5)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s6, s7));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getOutEdgesOf(s6)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s7, s8));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getInEdgesOf(s8)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::JUMP_EDGE);
    }

    CHECK(cfg->hasEdge(s9, s10));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getInEdgesOf(s10)) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::FALL_THROUGH_EDGE);
    }

    CHECK(cfg->hasEdge(s10, cfg->getExit()));
    for (const std::shared_ptr<graph::CFGEdge>& e : cfg->getInEdgesOf(cfg->getExit())) {
        CHECK_EQ(e->getKind(), graph::CFGEdge::Kind::EXIT_EDGE);
    }

    al::World::getLogger().Success("Finish testing get another method cfg ...");
}

TEST_CASE_FIXTURE(IRTestFixture, "testGetCFG3"
    * doctest::description("testing get the last method cfg")) {

    al::World::getLogger().Progress("Testing get the last method cfg ...");

    std::unordered_map<std::string, std::shared_ptr<air::Stmt>> stmtMap;
    for (const std::shared_ptr<air::Stmt>& s : ir4->getStmts()) {
        stmtMap.emplace(s->str(), s);
    }

    std::shared_ptr<air::Stmt> s1 = stmtMap.at("int result = 1;");
    std::shared_ptr<air::Stmt> s2 = stmtMap.at("int i = 1;");
    std::shared_ptr<air::Stmt> s3 = stmtMap.at("i <= n");
    std::shared_ptr<air::Stmt> s4 = stmtMap.at("result = result * i");
    std::shared_ptr<air::Stmt> s5 = stmtMap.at("i++");
    std::shared_ptr<air::Stmt> s6 = stmtMap.at("result");
    std::shared_ptr<air::Stmt> s7 = stmtMap.at("return result");

    std::shared_ptr<graph::CFG> cfg = ir4->getCFG();
    CHECK_EQ(cfg->getEdgeNum(), 9);

    CHECK(cfg->hasEdge(cfg->getEntry(), s1));
    CHECK(cfg->hasEdge(s1, s2));
    CHECK(cfg->hasEdge(s2, s3));
    CHECK(cfg->hasEdge(s3, s4));
    CHECK(cfg->hasEdge(s4, s5));
    CHECK(cfg->hasEdge(s5, s3));
    CHECK(cfg->hasEdge(s3, s6));
    CHECK(cfg->hasEdge(s6, s7));
    CHECK(cfg->hasEdge(s7, cfg->getExit()));

    al::World::getLogger().Success("Finish testing get the last method cfg ...");
}

TEST_SUITE_END();


