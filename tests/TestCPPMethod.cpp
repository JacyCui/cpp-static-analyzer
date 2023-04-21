#include "doctest.h"

#include "World.h"
#include "language/CPPMethod.h"

namespace al = analyzer;
namespace lang = al::language;

class CPPMethodTestFixture {
protected:
    std::shared_ptr<lang::CPPMethod> f1, f2, f3, f4, f5, f6, f7;
public:
    CPPMethodTestFixture()
    {
        al::World::initialize("resources/example01/src",
                              "resources/example01/include");
        const auto& allMethods = al::World::get().getAllMethods();
        f1 = allMethods.at("int main(int, const char **)");
        f2 = allMethods.at("int example01::Fib::fib(int)");
        f3 = allMethods.at("void example01::Fib::Fib(int)");
        f4 = allMethods.at("int example01::Fib::getNum()");
        const al::World& world = al::World::get();
        f5 = world.getMethodBySignature("int example01::Factor::factor(int)");
        f6 = world.getMethodBySignature("void example01::Factor::Factor(int)");
        f7 = world.getMethodBySignature("int example01::Factor::getNum()");
    }
};

TEST_SUITE_BEGIN("testCPPMethod");

TEST_CASE_FIXTURE(CPPMethodTestFixture, "testGetContainingFilePath"
    * doctest::description("testing method containing file getting")) {

    al::World::getLogger().Progress("Testing method containing file getting ...");

    CHECK_EQ(f1->getContainingFilePath(), "resources/example01/src/main.cpp");
    CHECK_EQ(f2->getContainingFilePath(), "resources/example01/src/fib/fib.cpp");
    CHECK_EQ(f3->getContainingFilePath(), "resources/example01/src/fib/fib.cpp");
    CHECK_EQ(f4->getContainingFilePath(), "resources/example01/src/fib/fib.cpp");
    CHECK_EQ(f5->getContainingFilePath(), "resources/example01/src/factor/factor.cpp");
    CHECK_EQ(f6->getContainingFilePath(), "resources/example01/src/factor/factor.cpp");
    CHECK_EQ(f7->getContainingFilePath(), "resources/example01/src/factor/factor.cpp");

    al::World::getLogger().Success("Finish testing method containing file getting ...");

}

TEST_CASE_FIXTURE(CPPMethodTestFixture, "testGetMethodSourceCode"
    * doctest::description("testing method source getting")) {

    al::World::getLogger().Progress("Testing method source getting ...");

    CHECK_EQ(f1->getMethodSourceCode(),
             R"(int main(int argc, const char* argv[]) {
    AbstractFactor* f1 = new Factor(10);
    int a = 8;
    int c = f1->factor(a);
    int d = f1->getNum();
    AbstractFib* f2 = new Fib(10);
    int e = 5;
    int f = f2->fib(e);
    int g = f2->getNum();
    return c + d + f + g;
})");

    CHECK_EQ(f2->getMethodSourceCode(),
             R"(int Fib::fib(int i) {
        int a = 0, b = 1;
        while (i > 0) {
            int tmp = b;
            b = a + b;
            a = tmp;
            i--;
        }
        return a;
    })");

    CHECK_EQ(f3->getMethodSourceCode(),
             R"(Fib::Fib(int n) {
        this->n = n;
    })");

    CHECK_EQ(f4->getMethodSourceCode(),
             R"(int Fib::getNum() {
        return n;
    })");

    CHECK_EQ(f5->getMethodSourceCode(),
             R"(int Factor::factor(int i) {
        int res = 1;
        while (i > 1) {
            res *= i;
            i--;
        }
        return res;
    })");

    CHECK_EQ(f6->getMethodSourceCode(),
             R"(Factor::Factor(int n) {
        this->n = n;
    })");

    CHECK_EQ(f7->getMethodSourceCode(),
             R"(int Factor::getNum() {
        return n;
    })");

    al::World::getLogger().Success("Finish testing method source getting ...");

}

TEST_CASE_FIXTURE(CPPMethodTestFixture, "testGetDeclarationInformation"
    * doctest::description("testing method declaration information get")) {

    al::World::getLogger().Progress("Testing method declaration information get ...");

    CHECK_EQ(f1->getParamCount(), 2);
    CHECK_EQ(f1->getParamName(0), "argc");
    CHECK_EQ(f1->getParamType(0)->getName(), "int");
    CHECK_EQ(f1->getParamName(1), "argv");
    CHECK_EQ(f1->getParamType(1)->getName(), "const char **");
    std::vector<std::string> types;
    for (const auto& type : f1->getParamTypes()) {
        types.emplace_back(type->getName());
    }
    std::sort(types.begin(), types.end());
    CHECK_EQ(types, std::vector<std::string>{"const char **" ,"int"});
    CHECK_EQ(f1->getReturnType()->getName(), "int");

    CHECK_EQ(f2->getParamCount(), 1);
    CHECK_EQ(f2->getParamName(0), "i");
    CHECK_EQ(f2->getParamType(0)->getName(), "int");
    CHECK_EQ(f2->getReturnType()->getName(), "int");

    CHECK_EQ(f3->getParamCount(), 1);
    CHECK_EQ(f3->getParamName(0), "n");
    CHECK_EQ(f3->getParamType(0)->getName(), "int");
    CHECK_EQ(f3->getReturnType()->getName(), "void");

    CHECK_EQ(f4->getParamCount(), 0);
    CHECK_EQ(f4->getReturnType()->getName(), "int");

    CHECK_EQ(f5->getParamCount(), 1);
    CHECK_EQ(f5->getParamName(0), "i");
    CHECK_EQ(f5->getParamType(0)->getName(), "int");
    CHECK_EQ(f5->getReturnType()->getName(), "int");

    CHECK_EQ(f6->getParamCount(), 1);
    CHECK_EQ(f6->getParamName(0), "n");
    CHECK_EQ(f6->getParamType(0)->getName(), "int");
    CHECK_EQ(f6->getReturnType()->getName(), "void");

    CHECK_EQ(f7->getParamCount(), 0);
    CHECK_EQ(f7->getReturnType()->getName(), "int");

    const auto* f8 = clang::dyn_cast<clang::CXXMethodDecl>(f7->getFunctionDecl());
    al::World::getLogger().Debug(f8->getThisObjectType().getAsString());
    al::World::getLogger().Debug(f7->getMethodSignatureAsString());
    al::World::getLogger().Debug(std::to_string(f8->getNumParams()));

    al::World::getLogger().Success("Finish testing method declaration information get ...");

}

TEST_CASE_FIXTURE(CPPMethodTestFixture, "testMethodKindCheck"
    * doctest::description("testing method kind checking")) {

    al::World::getLogger().Progress("Testing method kind checking ...");

    CHECK(f1->isGlobalMethod());
    CHECK_FALSE(f1->isClassStaticMethod());
    CHECK_FALSE(f1->isClassMemberMethod());
    CHECK_FALSE(f1->isVirtual());

    CHECK_FALSE(f2->isGlobalMethod());
    CHECK_FALSE(f2->isClassStaticMethod());
    CHECK(f2->isClassMemberMethod());
    CHECK(f2->isVirtual());

    CHECK_FALSE(f3->isGlobalMethod());
    CHECK_FALSE(f3->isClassStaticMethod());
    CHECK(f3->isClassMemberMethod());
    CHECK_FALSE(f3->isVirtual());

    CHECK_FALSE(f4->isGlobalMethod());
    CHECK_FALSE(f4->isClassStaticMethod());
    CHECK(f4->isClassMemberMethod());
    CHECK(f4->isVirtual());

    CHECK_FALSE(f5->isGlobalMethod());
    CHECK_FALSE(f5->isClassStaticMethod());
    CHECK(f5->isClassMemberMethod());
    CHECK(f5->isVirtual());

    CHECK_FALSE(f6->isGlobalMethod());
    CHECK_FALSE(f6->isClassStaticMethod());
    CHECK(f6->isClassMemberMethod());
    CHECK_FALSE(f6->isVirtual());

    CHECK_FALSE(f7->isGlobalMethod());
    CHECK_FALSE(f7->isClassStaticMethod());
    CHECK(f7->isClassMemberMethod());
    CHECK(f7->isVirtual());

    al::World::getLogger().Success("Finish testing method kind checking ...");

}

TEST_SUITE_END();
