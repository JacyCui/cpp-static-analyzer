#include <utility>
#include <algorithm>

#include "ir/IR.h"
#include "language/CPPMethod.h"

namespace analyzer::ir {

    DefaultIR::DefaultIR(const lang::CPPMethod& method,
                         std::vector<std::shared_ptr<Var>> params,
                         std::vector<std::shared_ptr<Var>> vars,
                         std::vector<std::shared_ptr<Stmt>> stmts,
                         const std::shared_ptr<graph::CFG>& cfg)
            :method(method), params(std::move(params)), vars(std::move(vars)),
                stmts(std::move(stmts)), cfg(cfg)
    {
        std::sort(this->stmts.begin(), this->stmts.end(),
            [](const std::shared_ptr<Stmt>& s1, const std::shared_ptr<Stmt>& s2) -> bool {
            return s1->getStartLine() < s2->getStartLine();
        });
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
        return vars;
    }

    std::vector<std::shared_ptr<Stmt>> DefaultIR::getStmts() const
    {
        return stmts;
    }

} // ir