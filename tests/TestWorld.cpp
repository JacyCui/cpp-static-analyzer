#include "doctest.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include "World.h"
#include "language/CPPMethod.h"

namespace al=analyzer;
namespace lang=al::language;

class WorldTestFixture {
private:
    static const al::World& initializeAndGet()
    {
        al::World::initialize("resources/example01/src",
                              "resources/example01/include");
        return al::World::get();
    }
protected:
    const al::World &world;
public:
    WorldTestFixture()
        :world(initializeAndGet())
    {

    }
};

TEST_SUITE_BEGIN("testWorld");

TEST_CASE_FIXTURE(WorldTestFixture, "testGetSourceCode"
    * doctest::description("test world building")) {

    al::World::getLogger().Progress("Testing world building ...");

    const std::unordered_map<std::string, std::string> &fileLists = world.getSourceCode();

    std::string p1("resources/example01/src/main.cpp");
    CHECK(fileLists.find(p1) != fileLists.end());
    std::ifstream f1(p1);
    CHECK(f1.is_open());
    CHECK_EQ(fileLists.at(p1), std::string(std::istreambuf_iterator<char>(f1),
                                           std::istreambuf_iterator<char>()));
    f1.close();

    std::string p2("resources/example01/src/factor/factor.cpp");
    CHECK(fileLists.find(p2) != fileLists.end());
    std::ifstream f2(p2);
    CHECK(f2.is_open());
    CHECK_EQ(fileLists.at(p2), std::string(std::istreambuf_iterator<char>(f2),
                                           std::istreambuf_iterator<char>()));

    std::string p3("resources/example01/src/fib/fib.cpp");
    CHECK(fileLists.find(p3) != fileLists.end());
    std::ifstream f3(p3);
    CHECK(f3.is_open());
    CHECK_EQ(fileLists.at(p3), std::string(std::istreambuf_iterator<char>(f3),
                                           std::istreambuf_iterator<char>()));

    al::World::getLogger().Success("Finish testing world building ...");

}

TEST_CASE_FIXTURE(WorldTestFixture, "testInitialization"
    * doctest::description("testing world initialization")) {

    al::World::getLogger().Progress("Testing world initialization ...");

    std::vector<std::string> fileList;
    for (const std::unique_ptr<clang::ASTUnit> &ast: world.getAstList()) {
        fileList.emplace_back(ast->getMainFileName().str());
    }
    std::sort(fileList.begin(), fileList.end());
    CHECK_EQ(fileList, std::vector<std::string>{
            "resources/example01/src/factor/factor.cpp",
            "resources/example01/src/fib/fib.cpp",
            "resources/example01/src/main.cpp"
    });

    al::World::getLogger().Success("Testing world initialization ...");

}

TEST_CASE_FIXTURE(WorldTestFixture, "testDumpAST"
    * doctest::description("testing pretty ast dumper")) {

    al::World::getLogger().Progress("Testing pretty ast dumper ...");

    std::string str;
    llvm::raw_string_ostream output(str);
    world.dumpAST(output);
    world.dumpAST(std::string("resources/example01/src/factor/factor.cpp"), output);
    uint64_t t1 = analyzer::World::getLogger().getOutStream()->tell();
    world.dumpAST(std::string("resources/example01/src/test.cpp"), output);
    uint64_t t2 = analyzer::World::getLogger().getOutStream()->tell();
    CHECK_NE(t1, t2);
    //al::World::getLogger().Debug(str);

    al::World::getLogger().Success("Finish testing pretty ast dumper ...");

}

TEST_CASE_FIXTURE(WorldTestFixture, "testGetAllMethods"
    * doctest::description("testing function list building")) {

    al::World::getLogger().Progress("Testing function list building ...");

    std::vector<std::string> signatureList;
    for (const auto& [sig, method]: world.getAllMethods()) {
        CHECK_EQ(sig, method->getMethodSignatureAsString());
        signatureList.emplace_back(method->getMethodSignatureAsString());
    }
    std::sort(signatureList.begin(), signatureList.end());
    CHECK_EQ(signatureList, std::vector<std::string>{
        "int example01::Factor::factor(int)",
        "int example01::Factor::getNum()",
        "int example01::Fib::fib(int)",
        "int example01::Fib::getNum()",
        "int main(int, const char **)",
        "void example01::Factor::Factor(int)",
        "void example01::Fib::Fib(int)"
    });

    al::World::getLogger().Success("Finish testing function list building ...");

}

TEST_SUITE_END();
