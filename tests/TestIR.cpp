#include "doctest.h"

#include "World.h"
#include "ir/IR.h"
#include "language/CPPMethod.h"

namespace al = analyzer;
namespace air = al::ir;

class IRTestFixture {
protected:
    std::shared_ptr<air::IR> ir1;
public:
    IRTestFixture() {
        al::World::initialize("resources/example02");
        const al::World& world = al::World::get();
        ir1 = world.getMethodBySignature("int main()")->getIR();
    }
};

TEST_SUITE_BEGIN("testIR");

TEST_CASE_FIXTURE(IRTestFixture, "testCFGDump" * doctest::description("test cfg dump")) {

}

TEST_SUITE_END();


