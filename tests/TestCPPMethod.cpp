#include "doctest.h"

#include "World.h"
#include "language/CPPMethod.h"

namespace al = analyzer;
namespace lang = al::language;

class CPPMethodTestFixture {
private:
    static const std::unordered_map<std::string, std::shared_ptr<lang::CPPMethod>>& initializeAndGet()
    {
        al::World::initialize(std::string("resources/example01/src"),
                              "resources/example01/include");
        return al::World::get().getAllMethods();
    }
protected:
    const std::unordered_map<std::string, std::shared_ptr<lang::CPPMethod>>& allMethods;
public:
    CPPMethodTestFixture()
        :allMethods(initializeAndGet())
    {

    }
};

TEST_SUITE_BEGIN("testCPPMethod");

TEST_CASE_FIXTURE(CPPMethodTestFixture, "testGetMethodSourceCode"
    * doctest::description("testing method source getting")) {

    CHECK_EQ(allMethods.at("int main(int, const char **)")->getMethodSourceCode(),
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

    CHECK_EQ(allMethods.at("int example01::Fib::fib(int)")->getMethodSourceCode(),
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

    CHECK_EQ(allMethods.at("void example01::Fib::Fib(int)")->getMethodSourceCode(),
             R"(Fib::Fib(int n) {
        this->n = n;
    })");

    CHECK_EQ(allMethods.at("int example01::Fib::getNum()")->getMethodSourceCode(),
             R"(int Fib::getNum() {
        return n;
    })");

    CHECK_EQ(allMethods.at("int example01::Factor::factor(int)")->getMethodSourceCode(),
             R"(int Factor::factor(int i) {
        int res = 1;
        while (i > 1) {
            res *= i;
            i--;
        }
        return res;
    })");

    CHECK_EQ(allMethods.at("void example01::Factor::Factor(int)")->getMethodSourceCode(),
             R"(Factor::Factor(int n) {
        this->n = n;
    })");

    CHECK_EQ(allMethods.at("int example01::Factor::getNum()")->getMethodSourceCode(),
             R"(int Factor::getNum() {
        return n;
    })");

}

TEST_CASE_FIXTURE(CPPMethodTestFixture, "testGetDeclarationInformation"
    * doctest::description("testing method declaration information get")) {

    CHECK_EQ(allMethods.at("int main(int, const char **)")->getParamCount(), 2);
    CHECK_EQ(allMethods.at("int main(int, const char **)")->getParamName(0), "argc");
    CHECK_EQ(allMethods.at("int main(int, const char **)")->getParamType(0).getName(), "int");
    CHECK_EQ(allMethods.at("int main(int, const char **)")->getParamName(1), "argv");
    CHECK_EQ(allMethods.at("int main(int, const char **)")->getParamType(1).getName(), "const char **");
    CHECK_EQ(allMethods.at("int main(int, const char **)")->getReturnType().getName(), "int");

    CHECK_EQ(allMethods.at("int example01::Fib::fib(int)")->getParamCount(), 1);
    CHECK_EQ(allMethods.at("int example01::Fib::fib(int)")->getParamName(0), "i");
    CHECK_EQ(allMethods.at("int example01::Fib::fib(int)")->getParamType(0).getName(), "int");
    CHECK_EQ(allMethods.at("int example01::Fib::fib(int)")->getReturnType().getName(), "int");

    CHECK_EQ(allMethods.at("void example01::Fib::Fib(int)")->getParamCount(), 1);
    CHECK_EQ(allMethods.at("void example01::Fib::Fib(int)")->getParamName(0), "n");
    CHECK_EQ(allMethods.at("void example01::Fib::Fib(int)")->getParamType(0).getName(), "int");
    CHECK_EQ(allMethods.at("void example01::Fib::Fib(int)")->getReturnType().getName(), "void");

    CHECK_EQ(allMethods.at("int example01::Fib::getNum()")->getParamCount(), 0);
    CHECK_EQ(allMethods.at("int example01::Fib::getNum()")->getReturnType().getName(), "int");

    CHECK_EQ(allMethods.at("int example01::Factor::factor(int)")->getParamCount(), 1);
    CHECK_EQ(allMethods.at("int example01::Factor::factor(int)")->getParamName(0), "i");
    CHECK_EQ(allMethods.at("int example01::Factor::factor(int)")->getParamType(0).getName(), "int");
    CHECK_EQ(allMethods.at("int example01::Factor::factor(int)")->getReturnType().getName(), "int");

    CHECK_EQ(allMethods.at("void example01::Factor::Factor(int)")->getParamCount(), 1);
    CHECK_EQ(allMethods.at("void example01::Factor::Factor(int)")->getParamName(0), "n");
    CHECK_EQ(allMethods.at("void example01::Factor::Factor(int)")->getParamType(0).getName(), "int");
    CHECK_EQ(allMethods.at("void example01::Factor::Factor(int)")->getReturnType().getName(), "void");

    CHECK_EQ(allMethods.at("int example01::Factor::getNum()")->getParamCount(), 0);
    CHECK_EQ(allMethods.at("int example01::Factor::getNum()")->getReturnType().getName(), "int");

}




TEST_SUITE_END();
