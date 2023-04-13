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

        explicit Type(const clang::QualType& quanType);

    private:

        clang::QualType quanType; ///< a qualified type
    };

} // language

#endif //STATIC_ANALYZER_TYPE_H
