#include <utility>

#include "ir/IR.h"
#include "language/CPPMethod.h"

namespace analyzer::ir {

    DefaultIR::DefaultIR(const lang::CPPMethod& method, std::shared_ptr<Var> thisVar,
                         std::vector<std::shared_ptr<Var>> params,
                         std::vector<std::shared_ptr<Var>> vars,
                         std::vector<std::shared_ptr<Var>> returnVars,
                         std::vector<std::shared_ptr<Stmt>> stmts)
            :method(method), thisVar(std::move(thisVar)), vars(std::move(vars)),
            params(std::move(params)), returnVars(std::move(returnVars)), stmts(std::move(stmts))
    {

    }

    const lang::CPPMethod& DefaultIR::getMethod() const
    {
        return method;
    }


} // ir