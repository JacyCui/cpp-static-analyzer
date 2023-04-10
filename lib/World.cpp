#include <filesystem>
#include <fstream>

#include <clang/Tooling/Tooling.h>

#include "World.h"

namespace fs = std::filesystem;

namespace analyzer {
    World* World::theWorld = nullptr;

    const World& World::get()
    {
        if (theWorld == nullptr) {
            throw runtime_error("The world is not initialized!");
        }
        return *theWorld;
    }

    unordered_map<string, string> loadSourceCodes(const string& sourceDir)
    {
        unordered_map<string, string> result;
        for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
            if (entry.is_regular_file()) {
                const fs::path& p = entry.path();
                const string& ext = p.extension().string();
                if (ext == ".cpp" || ext == ".cc" || ext == ".c" || ext == ".cxx") {
                    ifstream fileStream(p);
                    if (!fileStream.is_open()) {
                        throw runtime_error("Fail to open file: " + p.string());
                    }
                    result.insert_or_assign(
                            fs::relative(p).string(),
                            string(istreambuf_iterator<char>(fileStream),
                                    istreambuf_iterator<char>()));
                    fileStream.close();
                }
            }
        }
        return std::move(result);
    }

    void World::initialize(const string& sourceDir, const string& includeDir, const string& std)
    {
        theWorld = new World(loadSourceCodes(sourceDir),
                             vector<string>{"-I" + includeDir, "-std=" + std});
    }

    World::World(unordered_map<string, string>&& sourceCode, vector<string>&& args)
        :sourceCode(std::move(sourceCode)), args(std::move(args))
    {
        for (const auto& [filename, content] : this->sourceCode) {
            uint64_t t1 = llvm::errs().tell();
            unique_ptr<ASTUnit> p = tooling::buildASTFromCodeWithArgs(content, this->args, filename);
            uint64_t t2 = llvm::errs().tell();
            if (t1 != t2) {
                throw runtime_error("Detect syntax or semantic error in source file: " + filename);
            }
            astList.emplace_back(std::move(p));
        }
    }

    const vector<unique_ptr<ASTUnit>>& World::getAstList() const
    {
        return astList;
    }

} // analyzer
