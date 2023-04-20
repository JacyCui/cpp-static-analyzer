#include <utility>

#include "ir/Var.h"
#include "World.h"

namespace analyzer::ir {

    ClangVarWrapper::ClangVarWrapper(const lang::CPPMethod& method, const clang::VarDecl* varDecl)
        :method(method), varDecl(varDecl)
    {
        name = varDecl->getNameAsString();
        type = World::get().getTypeBuilder()->buildType(varDecl->getType());
    }

    const lang::CPPMethod& ClangVarWrapper::getMethod() const
    {
        return method;
    }

    const std::string& ClangVarWrapper::getName() const
    {
        return name;
    }

    std::shared_ptr<lang::Type> ClangVarWrapper::getType() const
    {
        return type;
    }

    std::uint64_t ClangVarWrapper::getIdentity() const
    {
        return reinterpret_cast<std::uint64_t>(varDecl);
    }

}
