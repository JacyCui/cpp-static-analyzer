#ifndef STATIC_ANALYZER_TYPE_H
#define STATIC_ANALYZER_TYPE_H

#include <clang/AST/Type.h>

namespace analyzer::language {

    /**
     * @class Type
     * @brief a class representing the type
     */
    class Type final {

    public:

        /**
         * @return the string representation of this type
         */
        std::string getName();

        // Functions below should not be called from clients

        /**
         * construct a wrapper for clang::QualType
         * @param qualType a clang qualType object
         */
        explicit Type(const clang::QualType& qualType);

    private:

        clang::QualType qualType; ///< a qualified type
    };

} // language

#endif //STATIC_ANALYZER_TYPE_H
