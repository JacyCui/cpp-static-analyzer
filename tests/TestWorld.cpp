#include "doctest.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include "World.h"

using namespace analyzer;

TEST_SUITE_BEGIN("World Tests");

TEST_CASE("testLoadSourceCodes"
    * doctest::description("test source code finding and reading")) {
    unordered_map<string, string> fileLists = loadSourceCodes(string("resources/example01/src"));

    string p1("resources/example01/src/main.cpp");
    CHECK(fileLists.find(p1) != fileLists.end());
    ifstream f1(p1);
    CHECK(f1.is_open());
    CHECK_EQ(fileLists.at(p1) , string(istreambuf_iterator<char>(f1),
                     istreambuf_iterator<char>()));
    f1.close();

    string p2("resources/example01/src/factor/factor.cpp");
    CHECK(fileLists.find(p2) != fileLists.end());
    ifstream f2(p2);
    CHECK(f2.is_open());
    CHECK_EQ(fileLists.at(p2) , string(istreambuf_iterator<char>(f2),
                                       istreambuf_iterator<char>()));

    string p3("resources/example01/src/fib/fib.cpp");
    CHECK(fileLists.find(p3) != fileLists.end());
    ifstream  f3(p3);
    CHECK(f3.is_open());
    CHECK_EQ(fileLists.at(p3) , string(istreambuf_iterator<char>(f3),
                                       istreambuf_iterator<char>()));
}

TEST_CASE("testInitialize"
    * doctest::description("test world initialization")) {
    World::initialize(string("resources/example01/src"),
                      "resources/example01/include", "c++11");
    const World& word = World::get();
    vector<string> fileList;
    for (const unique_ptr<ASTUnit>& ast : word.getAstList()) {;
        fileList.emplace_back(ast->getMainFileName().str());
    }
    sort(fileList.begin(), fileList.end());
    CHECK_EQ(fileList, vector<string>{
        "resources/example01/src/factor/factor.cpp",
        "resources/example01/src/fib/fib.cpp",
        "resources/example01/src/main.cpp"
    });

}


TEST_SUITE_END();

