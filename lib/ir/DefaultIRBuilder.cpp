#include <queue>

#include "ir/IR.h"
#include "ir/Stmt.h"
#include "language/CPPMethod.h"
#include "World.h"

namespace analyzer::ir {

    namespace lang = language;
    namespace graph = analysis::graph;

    std::shared_ptr<IR> DefaultIRBuilder::buildIR(const lang::CPPMethod& method) const
    {
        return DefaultIRBuilderHelper(method).build();
    }

    DefaultIRBuilderHelper::DefaultIRBuilderHelper(const lang::CPPMethod& method)
        :method(method)
    {

    }

    std::shared_ptr<IR> DefaultIRBuilderHelper::build()
    {
        World::getLogger().Progress("Using default ir builder to build ir for "
            + method.getMethodSignatureAsString() + " ...");
        World::getLogger().Info("Building parameter variables ...");
        buildParams();
        World::getLogger().Info("Building statements in this method ...");
        buildStmts();
        World::getLogger().Info("Building CFG for this method ...");
        std::shared_ptr<graph::DefaultCFG> cfg = std::make_shared<graph::DefaultCFG>();
        buildEdges(cfg);
        World::getLogger().Info("Encapsulating the above parts to form ir ...");
        for (const auto& [_, s]: stmts) {
            stmtVec.emplace_back(s);
        }
        std::shared_ptr<DefaultIR> myIR = std::make_shared<DefaultIR>(method, params, vars, stmtVec, cfg);
        cfg->setIR(myIR);
        World::getLogger().Success("IR of " + method.getMethodSignatureAsString() +
            " has been built by default ir builder ...");
        return myIR;
    }

    void DefaultIRBuilderHelper::buildParams()
    {
        std::size_t n = method.getParamCount();
        for (std::size_t i = 0; i < n; i++) {
            std::shared_ptr<Var> pi = World::get().getVarBuilder()->buildVar(method,
                method.getFunctionDecl()->getParamDecl(i));
            params.emplace_back(pi);
            vars.emplace(pi->getIdentity(), pi);
        }
    }

    void DefaultIRBuilderHelper::buildStmts()
    {
        for (const clang::CFGBlock* block: method.getClangCFG()->const_nodes()) {
            for (const clang::CFGElement& element: *block) {
                if (std::optional<clang::CFGStmt> cfgStmt = element.getAs<clang::CFGStmt>()) {
                    std::shared_ptr<Stmt> s = World::get().
                            getStmtBuilder()->buildStmt(method, cfgStmt->getStmt());
                    stmts.emplace(cfgStmt->getStmt(), s);
                    for (const std::shared_ptr<Var>& v : s->getDefs()) {
                        if (vars.find(v->getIdentity()) == vars.end()) {
                            vars.emplace(v->getIdentity(), v);
                        }
                    }
                    for (const std::shared_ptr<Var>& v : s->getUses()) {
                        if (vars.find(v->getIdentity()) == vars.end()) {
                            vars.emplace(v->getIdentity(), v);
                        }
                    }
                }
            }
        }
    }

    void DefaultIRBuilderHelper::buildEdges(std::shared_ptr<graph::DefaultCFG>& cfg)
    {

        std::shared_ptr<Stmt> entry = World::get().getStmtBuilder()->buildEmptyStmt(method);
        cfg->setEntry(entry);
        std::shared_ptr<Stmt> exit = World::get().getStmtBuilder()->buildEmptyStmt(method);
        cfg->setExit(exit);

        std::unordered_map<const clang::CFGBlock*, std::shared_ptr<Stmt>> emptyBlocks;

        for (const clang::CFGBlock* block: method.getClangCFG()->const_nodes()) {
            if (block == &method.getClangCFG()->getExit()) {
                continue;
            }

            std::shared_ptr<Stmt> source = nullptr;
            std::shared_ptr<Stmt> target = nullptr;

            // build fall through edges for this block
            for (const clang::CFGElement& element: *block) {
                if (std::optional<clang::CFGStmt> cfgStmt = element.getAs<clang::CFGStmt>()) {
                    target = stmts.at(cfgStmt->getStmt());
                    if (source) {
                        cfg->addEdge(std::make_shared<graph::DefaultCFGEdge>(
                                source, target, graph::CFGEdge::Kind::FALL_THROUGH_EDGE));
                    }
                    source = target;
                }
            }

            graph::CFGEdge::Kind kind = graph::CFGEdge::Kind::UNKNOWN_EDGE;

            if (block == &method.getClangCFG()->getEntry()) {
                kind = graph::CFGEdge::Kind::ENTRY_EDGE;
                source = entry;
            } else {
                kind = graph::CFGEdge::Kind::JUMP_EDGE;
                if (!target) {
                    if (emptyBlocks.find(block) == emptyBlocks.end()) {
                        emptyBlocks.emplace(block, World::get().getStmtBuilder()->buildEmptyStmt(method));
                        stmtVec.emplace_back(emptyBlocks.at(block));
                    }
                    target = emptyBlocks.at(block);
                }
                source = target;
            }

            for (const clang::CFGBlock* succ : block->succs()) {
                if (succ == &method.getClangCFG()->getExit()) {
                    cfg->addEdge(std::make_shared<graph::DefaultCFGEdge>
                            (source, exit, graph::CFGEdge::Kind::EXIT_EDGE));
                } else {
                    bool isEmpty = true;
                    for (const clang::CFGElement& element: *succ) {
                        if (std::optional<clang::CFGStmt> cfgStmt = element.getAs<clang::CFGStmt>()) {
                            target = stmts.at(cfgStmt->getStmt());
                            cfg->addEdge(std::make_shared<graph::DefaultCFGEdge>
                                                 (source, target, kind));
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty) {
                        if (emptyBlocks.find(succ) == emptyBlocks.end()) {
                            emptyBlocks.emplace(succ, World::get().getStmtBuilder()->buildEmptyStmt(method));
                            stmtVec.emplace_back(emptyBlocks.at(succ));
                        }
                        target = emptyBlocks.at(succ);
                        cfg->addEdge(std::make_shared<graph::DefaultCFGEdge>
                                             (source, target, kind));
                    }
                }
            }
        }
    }

}
