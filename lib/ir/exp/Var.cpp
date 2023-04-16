#include <utility>

#include "ir/Exp.h"

namespace analyzer::ir {

    Var::Var(const lang::CPPMethod& method, std::string name, const std::shared_ptr<lang::Type>& type)
        :method(method), name(std::move(name)), type(type)
    {

    }

    const lang::CPPMethod& Var::getMethod() const
    {
        return method;
    }

    const std::string& Var::getName() const
    {
        return name;
    }

    std::shared_ptr<lang::Type> Var::getType() const
    {
        return type;
    }

}
