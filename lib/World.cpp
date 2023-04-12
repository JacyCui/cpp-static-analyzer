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
            throw std::runtime_error("The world is not initialized!");
        }
        return *theWorld;
    }

    util::Logger& World::getLogger()
    {
        return logger;
    }

    void World::setLoggerOutstream(llvm::raw_ostream& os)
    {
        logger.setOutstream(&os);
    }

    void World::initialize(const std::string& sourceDir, const std::string& includeDir, const std::string& std)
    {
        llvm::outs() << "Start building the world...\n";
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
        llvm::outs() << "World building finished!\n";
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
                throw std::runtime_error("Detect syntax or semantic error in source file: " + filename);
            }
            astList.emplace_back(std::move(p));
        }
        mainMethod = nullptr;
        buildFunctionList();
    }

    World::~World() {

    }

    void World::buildFunctionList()
    {
        llvm::outs() << "Building function list...\n";
        std::unordered_set<std::string> signatureSet;
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
                llvm::outs() << "Building function " + sig + " ...\n";
                if (signatureSet.find(sig) == signatureSet.end()) {
                    signatureSet.insert(sig);
                    if (fd->getNameAsString() == "main") {
                        if (!mainMethod) {
                            mainMethod = std::make_shared<lang::CPPMethod>(ast, fd, sig);
                            allMethods.emplace_back(mainMethod);
                        } else {
                            throw std::runtime_error("Duplicate definition of main function!");
                        }
                    } else {
                        allMethods.emplace_back(std::make_shared<lang::CPPMethod>(ast, fd, sig));
                    }
                } else {
                    llvm::errs() << "Found another definition for " + sig + ", this definition is ignored!\n";
                }
            }
        }
        llvm::outs() << "Function list building finished!\n";
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
            llvm::errs() << fileName + " doesn't exist.\n";
        }
    }

    // ----------

    std::unordered_map<std::string, std::string> loadSourceCodes(const std::string& sourceDir)
    {
        llvm::outs() << "Loading source code from " + sourceDir + "...\n";
        std::unordered_map<std::string, std::string> result;
        for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
            if (entry.is_regular_file()) {
                const fs::path& p = entry.path();
                const std::string& ext = p.extension().string();
                if (ext == ".cpp" || ext == ".cc" || ext == ".c" || ext == ".cxx") {
                    std::ifstream fileStream(p);
                    if (!fileStream.is_open()) {
                        throw std::runtime_error("Fail to open file: " + p.string());
                    }
                    llvm::outs() << "Processing " << fs::relative(p).string() << " ...\n";
                    result.insert_or_assign(
                            fs::relative(p).string(),
                            std::string(std::istreambuf_iterator<char>(fileStream),
                                        std::istreambuf_iterator<char>()));
                    fileStream.close();
                }
            }
        }
        llvm::outs() << "Source loading finished!\n";
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
