#include "doctest.h"

#include "World.h"
#include "ir/IR.h"

namespace al = analyzer;
namespace air = al::ir;

class IRTestFixture {
protected:
    std::shared_ptr<air::IR> ir1, ir2;
public:
    IRTestFixture() {
        al::World::initialize("resources/example02");
        const al::World& world = al::World::get();
        ir1 = world.getMethodBySignature("int main(int, char **)")->getIR();
        ir2 = world.getMethodBySignature("int test1(int, int)")->getIR();
    }
};

TEST_SUITE_BEGIN("testIR");

TEST_CASE_FIXTURE(IRTestFixture, "testGetParams" * doctest::description("testing get method ir parameters")) {

    al::World::getLogger().Debug("Testing get method ir parameters ...");

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

}

TEST_CASE_FIXTURE(IRTestFixture, "testGetVars" * doctest::description("testing get method variables")) {

    al::World::getLogger().Debug("Testing get method variables ...");

    std::vector<std::shared_ptr<air::Var>> vars1 = ir1->getVars();
    std::unordered_map<std::string, std::string> output1;
    for (const std::shared_ptr<air::Var>& v : vars1) {
        output1.emplace(v->getName(), v->getType()->getName());
    }
    CHECK_EQ(output1, std::unordered_map<std::string, std::string>{
        {"a", "int"}, {"argc", "int"}, {"argv", "char **"}, {"b", "int"}, {"c", "int"}
    });

    std::vector<std::shared_ptr<air::Var>> vars2 = ir2->getVars();
    std::unordered_map<std::string, std::string> output2;
    for (const std::shared_ptr<air::Var>& v : vars2) {
        output2.emplace(v->getName(), v->getType()->getName());
    }
    CHECK_EQ(output2, std::unordered_map<std::string, std::string>{
        {"a", "int"},  {"b", "int"}, {"c", "int"}, {"d", "int"}
    });

}

TEST_CASE_FIXTURE(IRTestFixture, "test") {

}

TEST_SUITE_END();


