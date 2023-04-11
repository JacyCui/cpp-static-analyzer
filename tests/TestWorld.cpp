#include "doctest.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include "World.h"

namespace al=analyzer;

TEST_CASE("World Tests"
    * doctest::description("test world building")) {

    SUBCASE("testLoadSourceCodes") {
        INFO("testing source code finding and reading");
        std::unordered_map<std::string, std::string> fileLists = al::loadSourceCodes(
                std::string("resources/example01/src"));

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
    }

    al::World::initialize(std::string("resources/example01/src"),
                          "resources/example01/include");
    const al::World &word = al::World::get();

    SUBCASE("testInitialize") {
        INFO("testing world initialization");
        std::vector<std::string> fileList;
        for (const std::unique_ptr<clang::ASTUnit> &ast: word.getAstList()) { ;
            fileList.emplace_back(ast->getMainFileName().str());
        }
        sort(fileList.begin(), fileList.end());
        CHECK_EQ(fileList, std::vector<std::string>{
                "resources/example01/src/factor/factor.cpp",
                "resources/example01/src/fib/fib.cpp",
                "resources/example01/src/main.cpp"
        });
    }

    SUBCASE("testDumpAST") {
        INFO("testing pretty ast dumper");
        word.dumpAST(llvm::outs());
        word.dumpAST(std::string("resources/example01/src/factor/factor.cpp"), llvm::outs());
        uint64_t t1 = llvm::outs().tell(), t2 = llvm::errs().tell();
        word.dumpAST(std::string("resources/example01/src/test.cpp"), llvm::outs());
        uint64_t t3 = llvm::outs().tell(), t4 = llvm::errs().tell();
        CHECK_EQ(t1, t3);
        CHECK_NE(t2, t4);
    }

    SUBCASE("testBuildFunctionList") {
        INFO("testing function list building");
        std::vector<std::string> output;
        for (const auto &[sig, decl]: word.getFuncDecls()) {
            output.emplace_back(sig);
            llvm::outs() << sig << "\n";
            //llvm::outs() << decl->getBody()->
        }
    }
}

