#ifndef STATIC_ANALYZER_TYPE_H
#define STATIC_ANALYZER_TYPE_H

#include <clang/AST/Type.h>

namespace analyzer::language {

    /**
     * @class Type
     * @brief Represents the type
     */
    class Type {

    public:

        /**
         * @return the string representation of this type
         */
        [[nodiscard]] const std::string& getName() const;

        // Functions below should not be called from clients

        /**
         * @return the qualified clang type
         */
        [[nodiscard]] const clang::QualType& getQualType() const;

        /**
         * @brief construct a wrapper for clang::QualType
         * @param qualType a clang qualType object
         */
        explicit Type(const clang::QualType& qualType);

        Type(const Type&) = delete;
        Type& operator=(const Type&) = delete;

    private:

        std::string typeName; ///< type name string

        clang::QualType qualType; ///< a qualified type
    };

} // language

#endif //STATIC_ANALYZER_TYPE_H
