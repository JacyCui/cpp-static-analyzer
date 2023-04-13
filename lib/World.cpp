#include <filesystem>
#include <fstream>
#include <unordered_set>

#include <clang/Tooling/Tooling.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

#include "World.h"

namespace fs = std::filesystem;
namespace tl = clang::tooling;
namespace mt = clang::ast_matchers;

namespace analyzer {

    World* World::theWorld = nullptr;

    util::Logger World::logger(&llvm::outs());

    const World& World::get()
    {
        if (theWorld == nullptr) {
            logger.Error("The world is not initialized!");
            throw std::runtime_error("The world is not initialized!");
        }
        return *theWorld;
    }

    void World::initialize(const std::string& sourceDir, const std::string& includeDir, const std::string& std)
    {
        logger.Progress("Start building the world...");
        if (theWorld != nullptr) {
            delete theWorld;
            theWorld = nullptr;
        }
        if (includeDir.empty()) {
            theWorld = new World(loadSourceCodes(sourceDir),
                                 std::vector<std::string>{"-std=" + std});
        } else {
            theWorld = new World(loadSourceCodes(sourceDir),
                                 std::vector<std::string>{"-I" + includeDir, "-std=" + std});
        }
        logger.Success("World building finished!");
    }

    util::Logger& World::getLogger()
    {
        return logger;
    }

    void World::setLogger(util::Logger newLogger)
    {
        logger = newLogger;
    }

    //------

    World::World(std::unordered_map<std::string, std::string>&& sourceCode, std::vector<std::string>&& args)
        :sourceCode(std::move(sourceCode)), args(std::move(args))
    {
        for (const auto& [filename, content] : this->sourceCode) {
            uint64_t t1 = llvm::errs().tell();
            std::unique_ptr<clang::ASTUnit> p = tl::buildASTFromCodeWithArgs(content, this->args, filename);
            uint64_t t2 = llvm::errs().tell();
            if (t1 != t2) {
                logger.Error("Detect syntax or semantic error in source file: " + filename);
                throw std::runtime_error("Detect syntax or semantic error in source file: " + filename);
            }
            astList.emplace_back(std::move(p));
        }
        mainMethod = nullptr;
        buildMethodMap();
    }

    void World::buildMethodMap()
    {
        logger.Progress("Building function list...");
        for (const std::unique_ptr<clang::ASTUnit>& ast: astList) {

            class FunctionRegister: public mt::MatchFinder::MatchCallback {
            private:
                std::vector<const clang::FunctionDecl *> functions;

            public:
                void run(const mt::MatchFinder::MatchResult& Result) override {
                    if (const auto* fd = Result.Nodes.getNodeAs<clang::FunctionDecl>("function")) {
                        if (fd->isThisDeclarationADefinition() && fd->hasBody() && !fd->isImplicit()) {
                            functions.emplace_back(fd);
                        }
                    }
                }
                std::vector<const clang::FunctionDecl *> &getFunctions() {
                    return functions;
                }
            } functionRegister;

            mt::DeclarationMatcher funcDelMatcher = mt::functionDecl().bind("function");
            mt::MatchFinder finder;
            finder.addMatcher(funcDelMatcher, &functionRegister);
            finder.matchAST(ast->getASTContext());

            for (const clang::FunctionDecl* fd : functionRegister.getFunctions()) {
                std::string sig = lang::generateFunctionSignature(fd);
                logger.Info("Building function " + sig + " ...");
                if (allMethods.find(sig) == allMethods.end()) {
                    if (fd->getNameAsString() == "main") {
                        if (!mainMethod) {
                            mainMethod = std::make_shared<lang::CPPMethod>(ast, fd, sig);
                            allMethods.insert_or_assign(sig, mainMethod);
                        } else {
                            logger.Error("Duplicate definition of main function!");
                            throw std::runtime_error("Duplicate definition of main function!");
                        }
                    } else {
                        allMethods.insert_or_assign(sig, std::make_shared<lang::CPPMethod>(ast, fd, sig));
                    }
                } else {
                    logger.Warning("Found another definition for " + sig + ", this definition is ignored!");
                }
            }
        }
        logger.Success("Function list building finished!");
    }

    const std::unordered_map<std::string, std::string>& World::getSourceCode() const
    {
        return sourceCode;
    }

    const std::vector<std::unique_ptr<clang::ASTUnit>>& World::getAstList() const
    {
        return astList;
    }

    void World::dumpAST(llvm::raw_ostream& out) const
    {
        for (const std::unique_ptr<clang::ASTUnit>& ast: astList) {
            out << "----------------------------------------\n";
            out << ast->getMainFileName() << ": \n";
            out << "----------------------------------------\n";
            ast->getASTContext().getTranslationUnitDecl()->dump(out);
        }
    }

    void World::dumpAST(const std::string& fileName, llvm::raw_ostream& out) const
    {
        auto it = std::find_if(astList.begin(), astList.end(), [&](const auto& ast) -> bool {
            return ast->getMainFileName() == fileName;
        });
        if (it != astList.end()) {
            (*it)->getASTContext().getTranslationUnitDecl()->dump(out);
        } else {
            logger.Warning(fileName + " doesn't exist! AST dump operation is skipped.");
        }
    }

    const std::unordered_map<std::string, std::shared_ptr<lang::CPPMethod>>& World::getAllMethods() const
    {
        return allMethods;
    }

    const std::shared_ptr<lang::CPPMethod> World::getMainMethod() const
    {
        return mainMethod;
    }

    // ----------

    std::unordered_map<std::string, std::string> loadSourceCodes(const std::string& sourceDir)
    {
        World::getLogger().Progress("Loading source code from " + sourceDir + "...");
        std::unordered_map<std::string, std::string> result;
        for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
            if (entry.is_regular_file()) {
                const fs::path& p = entry.path();
                const std::string& ext = p.extension().string();
                if (ext == ".cpp" || ext == ".cc" || ext == ".c" || ext == ".cxx") {
                    std::ifstream fileStream(p);
                    if (!fileStream.is_open()) {
                        World::getLogger().Error("Fail to open file: " + p.string());
                        throw std::runtime_error("Fail to open file: " + p.string());
                    }
                    World::getLogger().Info("Processing " + fs::relative(p).string() + " ...");
                    result.insert_or_assign(
                            fs::relative(p).string(),
                            std::string(std::istreambuf_iterator<char>(fileStream),
                                        std::istreambuf_iterator<char>()));
                    fileStream.close();
                }
            }
        }
        World::getLogger().Success("Source loading finished!");
        return result;
    }

    namespace language {

        std::string generateFunctionSignature(const clang::FunctionDecl *functionDecl) {
            std::string result;
            result.append(functionDecl->getReturnType().getAsString());
            result.append(" ");
            result.append(functionDecl->getQualifiedNameAsString());
            result.append("(");
            bool firstParam = true;
            for (const clang::ParmVarDecl *param: functionDecl->parameters()) {
                if (!firstParam) {
                    result.append(", ");
                }
                result.append(param->getType().getAsString());
                firstParam = false;
            }
            result.append(")");
            return result;
        }

    }

} // analyzer
