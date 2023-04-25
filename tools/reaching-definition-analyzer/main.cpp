#include <llvm/Support/CommandLine.h>

#include "World.h"
#include "analysis/dataflow/ReachingDefinition.h"

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace graph = al::analysis::graph;
namespace df = al::analysis::dataflow;
namespace dfact = al::analysis::dataflow::fact;

static llvm::cl::opt<std::string> SourceDir("source-dir", llvm::cl::desc("directory of all source files"),
                                            llvm::cl::value_desc("directory"), llvm::cl::Required);
static llvm::cl::opt<std::string> IncludeDir("include-dir", llvm::cl::desc("directory of all header files"),
                                        llvm::cl::value_desc("directory"), llvm::cl::Optional);
static llvm::cl::opt<std::string> Std("standard", llvm::cl::desc("c++ language standard"),
                                 llvm::cl::value_desc("standard"), llvm::cl::Optional);

int main(int argc, const char **argv) {

    llvm::cl::SetVersionPrinter([](llvm::raw_ostream &out) -> void {
        llvm::outs() << "A Simple CPP Reaching Definition Static Analyzer\n";
        llvm::outs() << "Copyright (c) 2022-2022";
    });

    llvm::cl::ParseCommandLineOptions(argc, argv);

    if (IncludeDir.empty()) {
        al::World::initialize(SourceDir);
    } else if (Std.empty()) {
        al::World::initialize(SourceDir, IncludeDir);
    } else {
        al::World::initialize(SourceDir, IncludeDir, Std);
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

        al::World::getLogger().Info("-------------- Analysis Result -----------------");

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

        al::World::getLogger().Success("Finish reaching definition analysis for: " + signature);

    }

    return 0;
}

