#include "doctest.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include "World.h"
#include "language/CPPMethod.h"

namespace al=analyzer;
namespace lang=al::language;

TEST_CASE("testWorld"
    * doctest::description("test world building")) {

    al::World::initialize(std::string("resources/example01/src"),
                          "resources/example01/include");
    const al::World &world = al::World::get();

    //---------------------

    INFO("testing source code finding and reading");
    const std::unordered_map<std::string, std::string>& fileLists = world.getSourceCode();

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

    //---------------------

    INFO("testing world initialization");
    std::vector<std::string> fileList;
    for (const std::unique_ptr<clang::ASTUnit> &ast: world.getAstList()) {
        fileList.emplace_back(ast->getMainFileName().str());
    }
    sort(fileList.begin(), fileList.end());
    CHECK_EQ(fileList, std::vector<std::string>{
            "resources/example01/src/factor/factor.cpp",
            "resources/example01/src/fib/fib.cpp",
            "resources/example01/src/main.cpp"
    });

    //---------------------

    INFO("testing pretty ast dumper");
    std::string str;
    llvm::raw_string_ostream output(str);
    world.dumpAST(output);
    world.dumpAST(std::string("resources/example01/src/factor/factor.cpp"), output);
    uint64_t t1 = world.getLogger().getOutStream()->tell();
    world.dumpAST(std::string("resources/example01/src/test.cpp"), output);
    uint64_t t2 = world.getLogger().getOutStream()->tell();
    CHECK_NE(t1, t2);
    //world.getLogger().Debug(str);

    //---------------------

    INFO("testing function list building");
    for (const std::shared_ptr<lang::CPPMethod>& method : world.getAllMethods()) {
        world.getLogger().Debug(method->getMethodSignatureAsString());
        world.getLogger().Debug(method->getMethodSourceCode());
    }

}

