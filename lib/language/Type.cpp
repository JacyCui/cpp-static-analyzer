#include "language/Type.h"

namespace analyzer::language {

    Type::Type(const clang::QualType &qualType)
        : qualType(qualType)
    {
        typeName = qualType.getAsString();
    }

    const std::string& Type::getName() const
    {
        return typeName;
    }

    const clang::QualType& Type::getQualType() const
    {
        return qualType;
    }

} // language