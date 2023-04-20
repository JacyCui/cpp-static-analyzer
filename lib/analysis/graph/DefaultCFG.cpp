#include <utility>

#include "analysis/graph/CFG.h"

namespace analyzer::analysis::graph {

    DefaultCFGEdge::DefaultCFGEdge(std::shared_ptr<ir::Stmt> source, std::shared_ptr<ir::Stmt> target, Kind kind)
        :source(std::move(source)), target(std::move(target)), kind(kind)
    {

    }

    CFGEdge::Kind DefaultCFGEdge::getKind() const
    {
        return kind;
    }

    DefaultCFG::DefaultCFG()
    {

    }

    std::shared_ptr<ir::Stmt> DefaultCFGEdge::getSource() const
    {
        return source;
    }

    std::shared_ptr<ir::Stmt> DefaultCFGEdge::getTarget() const
    {
        return target;
    }

    std::shared_ptr<ir::Stmt> DefaultCFG::getEntry() const
    {
        return entry;
    }

    std::shared_ptr<ir::Stmt> DefaultCFG::getExit() const
    {
        return exit;
    }

    bool DefaultCFG::hasStmt(std::shared_ptr<ir::Stmt> stmt) const
    {
        return inEdges.find(stmt) != inEdges.end() || outEdges.find(stmt) != outEdges.end();
    }

    bool DefaultCFG::hasEdge(std::shared_ptr<ir::Stmt> source, std::shared_ptr<ir::Stmt> target) const
    {
        if (!hasStmt(source)) {
            return false;
        }
        return std::any_of(outEdges.at(source).begin(), outEdges.at(source).end(),
                           [&](const std::shared_ptr<CFGEdge>& outEdge){
            return outEdge->getTarget() == target;
        });
    }

    std::unordered_set<std::shared_ptr<ir::Stmt>> DefaultCFG::getPredsOf(std::shared_ptr<ir::Stmt> stmt) const
    {
        std::unordered_set<std::shared_ptr<ir::Stmt>> result;
        for (const std::shared_ptr<CFGEdge>& inEdge : getInEdgesOf(stmt)) {
            result.emplace(inEdge->getSource());
        }
        return result;
    }

    std::unordered_set<std::shared_ptr<ir::Stmt>> DefaultCFG::getSuccsOf(std::shared_ptr<ir::Stmt> stmt) const
    {
        std::unordered_set<std::shared_ptr<ir::Stmt>> result;
        for (const std::shared_ptr<CFGEdge>& outEdge : getInEdgesOf(stmt)) {
            result.emplace(outEdge->getTarget());
        }
        return result;
    }

    std::unordered_set<std::shared_ptr<CFGEdge>> DefaultCFG::getInEdgesOf(std::shared_ptr<ir::Stmt> stmt) const
    {
        if (inEdges.find(stmt) == inEdges.end()) {
            return {};
        }
        return inEdges.at(stmt);
    }

    std::unordered_set<std::shared_ptr<CFGEdge>> DefaultCFG::getOutEdgesOf(std::shared_ptr<ir::Stmt> stmt) const
    {
        if (outEdges.find(stmt) == outEdges.end()) {
            return {};
        }
        return outEdges.at(stmt);
    }

    void DefaultCFG::addEdge(const std::shared_ptr<CFGEdge>& edge)
    {
        if (inEdges.find(edge->getTarget()) == inEdges.end()
            || outEdges.find(edge->getSource()) == outEdges.end()) {
            inEdges.emplace(edge->getTarget(), std::unordered_set<std::shared_ptr<CFGEdge>>{edge});
            outEdges.emplace(edge->getSource(), std::unordered_set<std::shared_ptr<CFGEdge>>{edge});
            return;
        }
        inEdges.at(edge->getTarget()).emplace(edge);
        outEdges.at(edge->getSource()).emplace(edge);
    }

    void DefaultCFG::setEntry(const std::shared_ptr<ir::Stmt>& entry)
    {
        this->entry = entry;
    }

    void DefaultCFG::setExit(const std::shared_ptr<ir::Stmt>& exit)
    {
        this->exit = exit;
    }

    std::shared_ptr<ir::IR> DefaultCFG::getIR() const
    {
        return myIR.lock();
    }

    void DefaultCFG::setIR(const std::shared_ptr<ir::IR>& myIR)
    {
        this->myIR = myIR;
    }



}

