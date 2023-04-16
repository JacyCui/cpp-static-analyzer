#ifndef STATIC_ANALYZER_STMT_H
#define STATIC_ANALYZER_STMT_H

#include <memory>

#include "Exp.h"

namespace analyzer::ir {

    /**
     * @brief Representation of statements
     * @class Stmt
     */
    class Stmt {
    public:

        /**
         * @return the index of this Stmt in the container IR.
         */
        virtual std::size_t getIndex() const = 0;

        /**
         * @brief set the index of this Stmt
         * @param index
         */
        virtual void setIndex(std::size_t index) = 0;

    };

    class UnknownStmt: public Stmt {

    };

}

#endif //STATIC_ANALYZER_STMT_H
