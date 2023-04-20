#include "language/Type.h"

namespace analyzer::language {

    ClangTypeWrapper::ClangTypeWrapper(const clang::QualType &qualType)
        : qualType(qualType)
    {
        typeName = qualType.getAsString();
    }

    const std::string& ClangTypeWrapper::getName() const
    {
        return typeName;
    }

    const clang::QualType& ClangTypeWrapper::getQualType() const
    {
        return qualType;
    }

} // language