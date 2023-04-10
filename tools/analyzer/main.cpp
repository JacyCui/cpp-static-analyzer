#include <string>

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

#include "World.h"


using namespace llvm;
using namespace std;
using namespace analyzer;

static cl::opt<string> SourceDir("source-dir", cl::desc("directory of all source files"),
                                 cl::value_desc("directory"), cl::Required);
static cl::opt<string> IncludeDir("include-dir", cl::desc("directory of all header files"),
                                  cl::value_desc("directory"), cl::Optional);
static cl::opt<string> Std("standard", cl::desc("c++ language standard"),
                           cl::value_desc("standard"), cl::Optional);

int main(int argc, const char **argv) {

    cl::SetVersionPrinter([](raw_ostream &out) -> void {
        outs() << "CPP Static Analyzer\n";
        outs() << "Copyright (c) 2022-2022";
    });

    cl::ParseCommandLineOptions(argc, argv);

    return 0;
}

