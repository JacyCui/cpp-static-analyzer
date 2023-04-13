#include "language/Type.h"

namespace analyzer::language {

    Type::Type(const clang::QualType &qualType)
        : qualType(qualType)
    {

    }

    std::string Type::getName()
    {
        return qualType.getAsString();
    }

} // language