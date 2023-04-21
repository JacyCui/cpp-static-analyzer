#ifndef STATIC_ANALYZER_STMT_H
#define STATIC_ANALYZER_STMT_H

#include <memory>
#include <unordered_set>

#include "ir/Var.h"

namespace analyzer::ir {

    namespace lang = language;

    /**
     * @class Stmt
     * @brief Representation of statements
     */
    class Stmt {
    public:

        /**
         * @return the start line number of the statement (start from 1)
         */
        [[nodiscard]] virtual int getStartLine() const = 0;

        /**
         * @return the end line number of the statement (included)
         */
        [[nodiscard]] virtual int getEndLine() const = 0;

        /**
         * @return the start column number of the statement (start from 1)
         */
        [[nodiscard]] virtual int getStartColumn() const = 0;

        /**
         * @return the end column number of the statement (included)
         */
        [[nodiscard]] virtual int getEndColumn() const = 0;

        /**
         * @return the method that contains this Stmt
         */
        [[nodiscard]] virtual const lang::CPPMethod& getMethod() const = 0;

        /**
         * @return the variables defined in this Stmt
         */
        [[nodiscard]] virtual std::unordered_set<std::shared_ptr<Var>> getDefs() const = 0;

        /**
         * @return the variables used in this Stmt
         */
        [[nodiscard]] virtual std::unordered_set<std::shared_ptr<Var>> getUses() const = 0;

        /**
         * @return the string representation of this statement
         */
        [[nodiscard]] virtual std::string str() const = 0;

        /**
         * @return the clang AST node of this statement, nullptr if this statement is nop
         */
        [[nodiscard]] virtual const clang::Stmt* getClangStmt() const = 0;

    };

    class NopStmt: public Stmt {
    public:

        [[nodiscard]] int getStartLine() const override;

        [[nodiscard]] int getEndLine() const override;

        [[nodiscard]] int getStartColumn() const override;

        [[nodiscard]] int getEndColumn() const override;

        [[nodiscard]] const lang::CPPMethod& getMethod() const override;

        [[nodiscard]] std::unordered_set<std::shared_ptr<Var>> getDefs() const override;

        [[nodiscard]] std::unordered_set<std::shared_ptr<Var>> getUses() const override;

        [[nodiscard]] std::string str() const override;

        [[nodiscard]] virtual const clang::Stmt* getClangStmt() const override;

        explicit NopStmt(const lang::CPPMethod& method);

    private:

        const lang::CPPMethod& method;

    };


    /**
     * @class StmtBuilder
     * @brief the interface for statement builder
     */
    class StmtBuilder {
    public:

        /**
         * @brief build a statement in method based on a clang statement
         * @param method a cpp method that defines the statement
         * @param clangStmt the corresponding clang statement
         * @return a statement
         */
        [[nodiscard]] virtual std::shared_ptr<Stmt>
                buildStmt(const lang::CPPMethod &method, const clang::Stmt* clangStmt) = 0;

        /**
         * @brief build an empty statement in a given method
         * @param method a cpp method that defines the statement
         * @return an empty statement
         */
        [[nodiscard]] virtual std::shared_ptr<Stmt>
                buildEmptyStmt(const lang::CPPMethod &method) = 0;

    };

    /**
     * @class ClangStmtWrapper
     * @brief a implementation of Stmt by wrapping the clang Stmt*
     */
    class ClangStmtWrapper: public Stmt {
    public:

        [[nodiscard]] int getStartLine() const override;

        [[nodiscard]] int getEndLine() const override;

        [[nodiscard]] int getStartColumn() const override;

        [[nodiscard]] int getEndColumn() const override;

        [[nodiscard]] const lang::CPPMethod& getMethod() const override;

        [[nodiscard]] std::unordered_set<std::shared_ptr<Var>> getDefs() const override;

        [[nodiscard]] std::unordered_set<std::shared_ptr<Var>> getUses() const override;

        [[nodiscard]] std::string str() const override;

        [[nodiscard]] const clang::Stmt* getClangStmt() const override;

        ClangStmtWrapper(const lang::CPPMethod& method, const clang::Stmt* clangStmt);

    private:

        const clang::Stmt* clangStmt;

        const lang::CPPMethod& method;

        std::unordered_set<std::shared_ptr<Var>> uses;

        std::unordered_set<std::shared_ptr<Var>> defs;

        int startLine;

        int startColumn;

        int endLine;

        int endColumn;

        std::string source;

    };

    /**
     * @class DefaultStmtBuilder
     * @brief the default implementation of statement builder
     */
    class DefaultStmtBuilder: public StmtBuilder {

        [[nodiscard]] std::shared_ptr<Stmt>
            buildStmt(const lang::CPPMethod& method, const clang::Stmt* clangStmt) override;

        [[nodiscard]] std::shared_ptr<Stmt>
            buildEmptyStmt(const lang::CPPMethod &method) override;

    };

}

#endif //STATIC_ANALYZER_STMT_H
