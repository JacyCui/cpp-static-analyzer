#ifndef STATIC_ANALYZER_WORLD_H
#define STATIC_ANALYZER_WORLD_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <llvm/Support/raw_ostream.h>
#include <clang/Frontend/ASTUnit.h>

using namespace std;
using namespace clang;

namespace analyzer {

    class World {
    private:
        static World* theWorld;
    public:
        static const World& get();

        /**
         * Initialize the word the whole program, including ast, cfg and call graph
         * @param sourceDir the directory path of all source files
         * @param includeDir the directory path of all
         * @param std language standard, e.g. c++11, c99
         */
        static void initialize(const string& sourceDir, const string& includeDir, const string& std=string("c++11"));

    private:
        // sourcefile name -> sourcefile content
        const unordered_map<string, string> sourceCode;
        // compiler arguments
        vector<string> args;
        // asts of a program
        vector<unique_ptr<ASTUnit>> astList;
        World(unordered_map<string, string>&& sourceCode, vector<string>&& args);


    public:
        [[nodiscard]] const vector<unique_ptr<ASTUnit>>& getAstList() const;
    };

    /**
     * get c/cpp source codes recursively from a source file directory
     * @param sourceDir the directory containing all the source files
     * @return a map from filename(relative, end with .c / .cpp / .cxx / .cc) to its contents
     */
    unordered_map<string, string> loadSourceCodes(const string& sourceDir);


} // analyzer

#endif //STATIC_ANALYZER_WORLD_H
