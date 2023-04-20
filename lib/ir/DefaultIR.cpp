#include <utility>

#include "ir/IR.h"
#include "language/CPPMethod.h"

namespace analyzer::ir {

    DefaultIR::DefaultIR(const lang::CPPMethod& method,
                         const std::vector<std::shared_ptr<Var>>& params,
                         const std::unordered_map<std::uint64_t, std::shared_ptr<Var>>& vars,
                         const std::vector<std::shared_ptr<Stmt>>& stmts,
                         const std::shared_ptr<graph::CFG>& cfg)
            :method(method), params(std::move(params)), vars(std::move(vars)),
                stmts(std::move(stmts)), cfg(cfg)
    {

    }

    const lang::CPPMethod& DefaultIR::getMethod() const
    {
        return method;
    }

    std::shared_ptr<graph::CFG> DefaultIR::getCFG() const
    {
        return cfg;
    }

    std::vector<std::shared_ptr<Var>> DefaultIR::getParams() const
    {
        return params;
    }

    std::vector<std::shared_ptr<Var>> DefaultIR::getVars() const
    {
        std::vector<std::shared_ptr<Var>> result;
        result.reserve(vars.size());
        for (auto& [_, var] : vars) {
            result.emplace_back(var);
        }
        return result;
    }

    std::shared_ptr<Var> DefaultIR::getVarByIdentity(std::uint64_t id) const
    {
        if (vars.find(id) != vars.end()) {
            return vars.at(id);
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<Stmt>> DefaultIR::getStmts() const
    {
        return stmts;
    }

} // ir