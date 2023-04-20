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
        [[nodiscard]] virtual const std::string& getName() const = 0;

    };

    /**
     * @class ClangTypeWrapper
     * @brief a simple implementation of type by wrapping clang qualified type
     */
    class ClangTypeWrapper: public Type {
    public:

        [[nodiscard]] const std::string& getName() const override;

        /**
         * @return the qualified clang type
         */
        [[nodiscard]] const clang::QualType& getQualType() const;

        /**
         * @brief construct a wrapper for clang::QualType
         * @param qualType a clang qualType object
         */
        explicit ClangTypeWrapper(const clang::QualType& qualType);

    private:
        std::string typeName; ///< type name string
        clang::QualType qualType; ///< a qualified type
    };

    /**
     * @class TypeBuilder
     * @brief build type from clang qual type
     */
    class TypeBuilder {
    public:

        /**
         * @brief build type from clang qual type
         * @param qualType a clang qual type
         * @return a type used in this analyzer
         */
        [[nodiscard]] virtual std::shared_ptr<Type> buildType(const clang::QualType& qualType) = 0;

    };

    /**
     * @class DefaultTypeBuilder
     * @brief the default implementation of type builder
     */
    class DefaultTypeBuilder: public TypeBuilder {
    public:

        [[nodiscard]] std::shared_ptr<Type> buildType(const clang::QualType& qualType) override;

    };

} // language

#endif //STATIC_ANALYZER_TYPE_H
