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

    std::shared_ptr<ir::Stmt> DefaultCFGEdge::getSource() const
    {
        return source;
    }

    std::shared_ptr<ir::Stmt> DefaultCFGEdge::getTarget() const
    {
        return target;
    }

    DefaultCFG::DefaultCFG()
        :edgeNum(0)
    {

    }

    std::shared_ptr<ir::Stmt> DefaultCFG::getEntry() const
    {
        return entry;
    }

    std::shared_ptr<ir::Stmt> DefaultCFG::getExit() const
    {
        return exit;
    }

    bool DefaultCFG::hasEdge(std::shared_ptr<ir::Stmt> source, std::shared_ptr<ir::Stmt> target) const
    {
        std::unordered_set<std::shared_ptr<ir::Stmt>>&& succs = getSuccsOf(source);
        return succs.find(target) != succs.end();
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
        for (const std::shared_ptr<CFGEdge>& outEdge : getOutEdgesOf(stmt)) {
            result.emplace(outEdge->getTarget());
        }
        return result;
    }

    std::unordered_set<std::shared_ptr<CFGEdge>> DefaultCFG::getInEdgesOf(std::shared_ptr<ir::Stmt> stmt) const
    {
        std::unordered_set<std::shared_ptr<CFGEdge>> result;
        result.reserve(inEdges.count(stmt));
        auto range = inEdges.equal_range(stmt);
        for (auto&& it = range.first; it != range.second; it++) {
            result.emplace(it->second);
        }
        return result;
    }

    std::unordered_set<std::shared_ptr<CFGEdge>> DefaultCFG::getOutEdgesOf(std::shared_ptr<ir::Stmt> stmt) const
    {
        std::unordered_set<std::shared_ptr<CFGEdge>> result;
        result.reserve(outEdges.count(stmt));
        auto range = outEdges.equal_range(stmt);
        for (auto&& it = range.first; it != range.second; it++) {
            result.emplace(it->second);
        }
        return result;
    }

    void DefaultCFG::addEdge(const std::shared_ptr<CFGEdge>& edge)
    {
        inEdges.emplace(edge->getTarget(), edge);
        outEdges.emplace(edge->getSource(), edge);
        edgeNum++;
    }

    std::size_t DefaultCFG::getEdgeNum() const
    {
        return edgeNum;
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

