#ifndef STATIC_ANALYZER_WORLD_H
#define STATIC_ANALYZER_WORLD_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <llvm/Support/raw_ostream.h>
#include <clang/Frontend/ASTUnit.h>

#include "language/CPPMethod.h"
#include "ir/IR.h"
#include "util/Logger.h"

namespace analyzer {

    namespace lang = language;

    /**
     * @class WorldBuilder
     * @brief Interface for the world builder
     */
    class WorldBuilder {
    public:

        /**
         * @brief build the world
         */
        virtual void build() = 0;

        virtual ~WorldBuilder() = default;

    };

    /**
     * @class World
     * @brief Manages the whole-program information of the program being analyzed.
     */
    class World final: public WorldBuilder {
    public:

        /**
         * @brief Initialize the word the whole program, including ast, cfg and call graph
         * @param sourceDir the directory path of all source files
         * @param includeDir the directory path of all
         * @param std language standard, e.g. c++98, c++11, c99
         */
        static void initialize(const std::string& sourceDir, const std::string& includeDir="",
                               const std::string& std=std::string("c++98"));

        /**
         * @brief must be called after calling {@code initialize}
         * @return const reference to the world instance created by {@code initialize}
         */
        static const World& get();

        /**
         * @return the current logger of the entire world
         */
        static util::Logger& getLogger();

        /**
         * @param newLogger a new logger to be used from now on
         */
        static void setLogger(util::Logger newLogger);

    public:

        void build() override;

        /**
         * @return a map from source file path to source file content
         */
        [[nodiscard]] const std::unordered_map<std::string, std::string>& getSourceCode() const;

        /**
         * @return the ASTUnit list of the whole program
         */
        [[nodiscard]] const std::vector<std::unique_ptr<clang::ASTUnit>>& getAstList() const;

        /**
         * @brief pretty dump asts of all source codes in the world into a stream
         * @param[out] out the llvm raw out stream (e.g. outs(), errs() or other user defined streams)
         */
        void dumpAST(llvm::raw_ostream& out) const;

        /**
         * @brief pretty dump asts of a given source file
         * @param fileName relative path of the source file from current working directory
         * @param[out] out the llvm raw out stream (e.g. outs(), errs() or other user defined streams)
         */
        void dumpAST(const std::string& fileName, llvm::raw_ostream& out) const;

        /**
         * @return return a vector of all methods in the program
         */
        [[nodiscard]] const std::unordered_map<std::string, std::shared_ptr<lang::CPPMethod>>& getAllMethods() const;

        /**
         * get the cpp method by it's method
         * @param signature signature string (e.g. int add(int, int))
         * @return a cpp method, nullptr if it doesn't exist
         */
        [[nodiscard]] std::shared_ptr<lang::CPPMethod> getMethodBySignature(const std::string& signature) const;

        /**
         * @return return the main method (nullptr if there's no main method)
         */
        [[nodiscard]] std::shared_ptr<lang::CPPMethod> getMainMethod() const;

        /**
         * @return the global ir builder of this world
         */
        [[nodiscard]] const std::unique_ptr<ir::IRBuilder>& getIRBuilder() const;

        /**
         * @return the global type builder of this world
         */
        [[nodiscard]] const std::unique_ptr<lang::TypeBuilder>& getTypeBuilder() const;

        /**
         * @return the global variable builder of this world
         */
        [[nodiscard]] const std::unique_ptr<ir::VarBuilder>& getVarBuilder() const;

        /**
         * @return the global statement builder of this world
         * @return
         */
        [[nodiscard]] const std::unique_ptr<ir::StmtBuilder>& getStmtBuilder() const;

    private:

        static World* theWorld; ///< the only world instance in the program

        static util::Logger logger; ///< the logger for the whole world

    private:

        std::unordered_map<std::string, std::string> sourceCode; ///< sourcefile name -> sourcefile content

        std::vector<std::string> args; ///< compiler arguments

        std::vector<std::unique_ptr<clang::ASTUnit>> astList; ///< asts of a program

        std::unordered_map<std::string, std::shared_ptr<lang::CPPMethod>> allMethods; ///< all cpp methods in the program

        std::shared_ptr<lang::CPPMethod> mainMethod; ///< main method

        std::unique_ptr<ir::IRBuilder> irBuilder; ///< global ir builder

        std::unique_ptr<lang::TypeBuilder> typeBuilder; ///< global type builder

        std::unique_ptr<ir::VarBuilder> varBuilder; ///< global variable builder

        std::unique_ptr<ir::StmtBuilder> stmtBuilder; ///< global statement builder

        /**
         * @brief Construct the world
         * @param sourceCode cpp or c source file and corresponding code string
         * @param args compiler arguments
         */
        World(std::unordered_map<std::string, std::string>&& sourceCode,
              std::vector<std::string>&& args);

        /**
         * @brief build a map from method signature to CPPMethod
         */
        void buildMethodMap();

        World(const World&) = delete;

        World& operator=(const World&) = delete;
    };

    /**
     * @brief get c/cpp source codes recursively from a source file directory
     * @param sourceDir the directory containing all the source files
     * @return a map from filename(relative, end with .c / .cpp / .cxx / .cc) to its contents
     */
    std::unordered_map<std::string, std::string> loadSourceCodes(const std::string& sourceDir);

    namespace language {

        /**
         * @brief generate string signature of a function decl ast
         * @param functionDecl function declaration ast
         * @return a string representation of signature
         */
        std::string generateFunctionSignature(const clang::FunctionDecl* functionDecl);

    }

} // analyzer

#endif //STATIC_ANALYZER_WORLD_H
