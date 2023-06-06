#include "CLI11.h"

#include "World.h"
#include "analysis/dataflow/ConstantPropagation.h"

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace graph = al::analysis::graph;
namespace df = al::analysis::dataflow;
namespace dfact = al::analysis::dataflow::fact;

int main(int argc, const char **argv)
{

    CLI::App app("A Simple CPP Constant Propagation Static Analyzer\nCopyright (c) 2023-2023");

    std::string sourceDir, includeDir, std;

    app.add_option("-S,--source-dir,", sourceDir,
                   "directory of all source files")->required();

    app.add_option("-I,--include-dir", includeDir,
                   "directory of all header files");

    app.add_option("--std,--standard", std,
                   "c++ language standard (support all standards that clang supports)");

    CLI11_PARSE(app, argc, argv);

    if (includeDir.empty() && std.empty()) {
        al::World::initialize(sourceDir);
    } else if (std.empty()) {
        al::World::initialize(sourceDir, includeDir);
    } else {
        al::World::initialize(sourceDir, includeDir, std);
    }

    std::unique_ptr<cf::AnalysisConfig> analysisConfig
            = std::make_unique<cf::DefaultAnalysisConfig>("constant propagation analysis");

    std::unique_ptr<df::ConstantPropagation> cp = std::make_unique<df::ConstantPropagation>(analysisConfig);

    const al::World& world = al::World::get();
    for (const auto& [signature, method] : world.getAllMethods()) {
        al::World::getLogger().Progress("Start constant propagation analysis for: " + signature);

        std::string fileName = method->getContainingFilePath();

        std::shared_ptr<air::IR> myIR = method->getIR();

        std::shared_ptr<dfact::DataflowResult<df::CPFact>> result = cp->analyze(myIR);

        al::World::getLogger().Info("-------------- Analysis Result of " + signature + " -----------------");

        for (const std::shared_ptr<air::Stmt>& stmt : myIR->getStmts()) {
            al::World::getLogger().Info("* " + fileName
                                        + " " + std::to_string(stmt->getStartLine()) + ": " + stmt->str());
            al::World::getLogger().Info("    In: ");
            result->getInFact(stmt)->forEach(
                    [&](const std::shared_ptr<air::Var>& k, const std::shared_ptr<df::CPValue>& v)
                     {
                         al::World::getLogger().Info("        " + k->getName() + ": " + v->str());
                     });
            al::World::getLogger().Info("    Out: ");
            result->getOutFact(stmt)->forEach(
                    [&](const std::shared_ptr<air::Var>& k, const std::shared_ptr<df::CPValue>& v)
                    {
                        al::World::getLogger().Info("        " + k->getName() + ": " + v->str());
                    });
        }

        al::World::getLogger().Info("-------------------------------------------------");

        al::World::getLogger().Success("Finish constant propagation analysis for: " + signature);

    }

    return 0;

}


