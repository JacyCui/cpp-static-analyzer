#include "language/Type.h"

namespace analyzer::language {

    Type::Type(const clang::QualType &quanType)
        :quanType(quanType)
    {

    }

    std::string Type::getName()
    {
        return quanType.getAsString();
    }

} // language