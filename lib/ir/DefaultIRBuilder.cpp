#include "ir/IR.h"
#include "language/CPPMethod.h"
#include "World.h"

namespace analyzer::ir {

    std::shared_ptr<IR> DefaultIRBuilder::buildIR(const lang::CPPMethod& method) const
    {
        return DefaultIRBuilderHelper(method).build();
    }

    DefaultIRBuilderHelper::DefaultIRBuilderHelper(const lang::CPPMethod& method)
        :method(method)
    {
        for (const clang::CFGBlock* block: method.getClangCFG()->const_nodes()) {
            World::getLogger().Debug("BlockID: " + std::to_string(block->getBlockID()));
            for (const clang::CFGElement& element: *block) {
                if (std::optional<clang::CFGStmt> cfgStmt = element.getAs<clang::CFGStmt>()) {
                    World::getLogger().Debug(std::string("Stmt Type: ") + cfgStmt->getStmt()->getStmtClassName());
                }
            }
        }
    }

    std::shared_ptr<IR> DefaultIRBuilderHelper::build()
    {
        return nullptr;
    }
}
