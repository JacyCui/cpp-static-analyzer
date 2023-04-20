#include "language/Type.h"

namespace analyzer::language {

    std::shared_ptr<Type> DefaultTypeBuilder::buildType(const clang::QualType& qualType)
    {
        return std::make_shared<ClangTypeWrapper>(qualType);
    }

}
