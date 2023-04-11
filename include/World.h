#ifndef STATIC_ANALYZER_WORLD_H
#define STATIC_ANALYZER_WORLD_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <llvm/Support/raw_ostream.h>
#include <clang/Frontend/ASTUnit.h>

namespace analyzer {

    /**
     * The program information
     */
    class World final {
    private:
        static World* theWorld;
    public:
        /**
         * must be called after calling {@code initialize}
         * @return const reference to the world instance created by {@code initialize}
         */
        static const World& get();

        /**
         * Initialize the word the whole program, including ast, cfg and call graph
         * @param sourceDir the directory path of all source files
         * @param includeDir the directory path of all
         * @param std language standard, e.g. c++11, c99
         */
        static void initialize(const std::string& sourceDir, const std::string& includeDir="", const std::string& std=std::string("c++98"));

    private:
        // sourcefile name -> sourcefile content
        std::unordered_map<std::string, std::string> sourceCode;
        // compiler arguments
        std::vector<std::string> args;
        // asts of a program
        std::vector<std::unique_ptr<clang::ASTUnit>> astList;
        // all function declarations with definition
        std::unordered_map<std::string, const clang::FunctionDecl*> funcDecls;
        // main function
        const clang::FunctionDecl* mainFuncDecl;

        /**
         * Construct the world, will only be called once.
         * @param sourceCode map from source code filename to source code string
         * @param args compiler arguments
         */
        World(std::unordered_map<std::string, std::string>&& sourceCode, std::vector<std::string>&& args);
        ~World();

        void buildFunctionList();

    public:

        /**
         * @return the ASTUnit list of the whole program
         */
        [[nodiscard]] const std::vector<std::unique_ptr<clang::ASTUnit>>& getAstList() const;

        /**
         * pretty dump asts of all source codes in the world
         * @param out the llvm raw out stream (e.g. outs(), errs() or other user defined streams)
         */
        void dumpAST(llvm::raw_ostream& out) const;

        /**
         * pretty dump asts of a given source file
         * @param fileName relative path of the source file from current working directory
         * @param out the llvm raw out stream (e.g. outs(), errs() or other user defined streams)
         */
        void dumpAST(const std::string& fileName, llvm::raw_ostream& out) const;


        [[nodiscard]] const std::unordered_map<std::string, const clang::FunctionDecl*>& getFuncDecls() const;

    };

    /**
     * get c/cpp source codes recursively from a source file directory
     * @param sourceDir the directory containing all the source files
     * @return a map from filename(relative, end with .c / .cpp / .cxx / .cc) to its contents
     */
    std::unordered_map<std::string, std::string> loadSourceCodes(const std::string& sourceDir);

    std::string generateFunctionSignature(const clang::FunctionDecl* functionDecl);

} // analyzer

#endif //STATIC_ANALYZER_WORLD_H
