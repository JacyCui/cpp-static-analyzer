#include "CLI11.h"

#include "World.h"
#include "analysis/dataflow/ReachingDefinition.h"

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace graph = al::analysis::graph;
namespace df = al::analysis::dataflow;
namespace dfact = al::analysis::dataflow::fact;

int main(int argc, const char **argv)
{

    CLI::App app("A Simple CPP Reaching Definition Static Analyzer\nCopyright (c) 2023-2023");

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
            = std::make_unique<cf::DefaultAnalysisConfig>("reaching definition analysis");

    std::unique_ptr<df::ReachingDefinition> rd = std::make_unique<df::ReachingDefinition>(analysisConfig);

    const al::World& world = al::World::get();
    for (const auto& [signature, method] : world.getAllMethods()) {
        al::World::getLogger().Progress("Start reaching definition analysis for: " + signature);

        std::string fileName = method->getContainingFilePath();

        std::shared_ptr<air::IR> myIR = method->getIR();

        std::shared_ptr<dfact::DataflowResult<dfact::SetFact<air::Stmt>>> result =
            rd->analyze(myIR);

        al::World::getLogger().Info("-------------- Analysis Result of " + signature + " -----------------");

        for (const std::shared_ptr<air::Stmt>& stmt : myIR->getStmts()) {
            al::World::getLogger().Info("* " + fileName
                + " " + std::to_string(stmt->getStartLine()) + ": " + stmt->str());
            al::World::getLogger().Info("    In: ");
            result->getInFact(stmt)->forEach([&](const std::shared_ptr<air::Stmt>& s)
            {
                al::World::getLogger().Info("        " + fileName
                    + " " + std::to_string(s->getStartLine()) + ": " + s->str());
            });
            al::World::getLogger().Info("    Out: ");
            result->getOutFact(stmt)->forEach([&](const std::shared_ptr<air::Stmt>& s)
            {
                al::World::getLogger().Info("        " + fileName
                    + " " + std::to_string(s->getStartLine()) + ": " + s->str());
            });
        }

        al::World::getLogger().Info("-------------------------------------------------");

        al::World::getLogger().Success("Finish reaching definition analysis for: " + signature);

    }

    return 0;

}

