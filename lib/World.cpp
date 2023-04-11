#include <filesystem>
#include <fstream>

#include <clang/Tooling/Tooling.h>

#include "World.h"

namespace fs = std::filesystem;
namespace tl = clang::tooling;

namespace analyzer {
    World* World::theWorld = nullptr;

    const World& World::get()
    {
        if (theWorld == nullptr) {
            throw std::runtime_error("The world is not initialized!");
        }
        return *theWorld;
    }

    std::unordered_map<std::string, std::string> loadSourceCodes(const std::string& sourceDir)
    {
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
                    result.insert_or_assign(
                            fs::relative(p).string(),
                            std::string(std::istreambuf_iterator<char>(fileStream),
                                   std::istreambuf_iterator<char>()));
                    fileStream.close();
                }
            }
        }
        return std::move(result);
    }

    void World::initialize(const std::string& sourceDir, const std::string& includeDir, const std::string& std)
    {
        if (theWorld != nullptr) {
            delete theWorld;
            theWorld = nullptr;
        }
        theWorld = new World(loadSourceCodes(sourceDir),
                             std::vector<std::string>{"-I" + includeDir, "-std=" + std});
    }

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
    }

    World::~World() {

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
            llvm::errs() << fileName + "doesn't exist.\n";
        }
    }

} // analyzer
