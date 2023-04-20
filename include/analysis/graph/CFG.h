#ifndef STATIC_ANALYZER_CFG_H
#define STATIC_ANALYZER_CFG_H

#include <unordered_map>
#include <unordered_set>

namespace analyzer::ir {
    class IR;
    class Stmt;
}

namespace analyzer::analysis::graph {

    /**
     * @brief the interface for a CFG Edge
     * @class CFGEdge
     */
    class CFGEdge {
    public:
        /**
         * @brief edge kind enumeration
         */
        enum class Kind {
            ENTRY_EDGE,
            EXIT_EDGE,
            FALL_THROUGH_EDGE,
            JUMP_EDGE,
            UNKNOWN_EDGE ///< unknown kind of cfg edge
        };

        /**
         * @return the kind of the CFG edge
         */
        [[nodiscard]] virtual Kind getKind() const = 0;

        /**
         *
         * @return the source statement of this CFG edge
         */
        [[nodiscard]] virtual std::shared_ptr<ir::Stmt> getSource() const = 0;

        /**
         * @return the target statements of this CFG edge
         */
        [[nodiscard]] virtual std::shared_ptr<ir::Stmt> getTarget() const = 0;

    };

    /**
     * @brief the interface for a statement CFG
     * @class CFG
     */
    class CFG {
    public:

        /**
         * @return the entry nop statement of this CFG
         */
        [[nodiscard]] virtual std::shared_ptr<ir::Stmt> getEntry() const = 0;

        /**
         * @return the exit nop statement of this CFG
         */
        [[nodiscard]] virtual std::shared_ptr<ir::Stmt> getExit() const = 0;

        /**
         * @brief check the existence of a statement (check by identity, rather than equality)
         * @param stmt the statement to be checked
         * @return true if exists, otherwise false
         */
        [[nodiscard]] virtual bool hasStmt(std::shared_ptr<ir::Stmt> stmt) const = 0;

        /**
         * @brief check the existence of a CFG edge
         * @param source the source statement
         * @param target the target statement
         * @return true if there's  CFG edge from source to target
         */
        [[nodiscard]] virtual bool
            hasEdge(std::shared_ptr<ir::Stmt> source, std::shared_ptr<ir::Stmt> target) const = 0;

        /**
         * @brief get the predecessor statements of a given statement
         * @param stmt a statement (check by identity)
         * @return a vector of predecessors
         */
        [[nodiscard]] virtual std::unordered_set<std::shared_ptr<ir::Stmt>>
            getPredsOf(std::shared_ptr<ir::Stmt> stmt) const = 0;

        /**
         * @brief get the successor statements of a given statement
         * @param stmt a statement (check by identity)
         * @return a vector of successors
         */
        [[nodiscard]] virtual std::unordered_set<std::shared_ptr<ir::Stmt>>
            getSuccsOf(std::shared_ptr<ir::Stmt> stmt) const = 0;

        /**
         * @brief get the in edges of a given statement
         * @param stmt a statement (check by identity)
         * @return a vector of in edges
         */
        [[nodiscard]] virtual std::unordered_set<std::shared_ptr<CFGEdge>>
            getInEdgesOf(std::shared_ptr<ir::Stmt> stmt) const = 0;

        /**
         * @brief get the out edges of a given statement
         * @param stmt a statement (check by identity)
         * @return a vector of out edges
         */
        [[nodiscard]] virtual std::unordered_set<std::shared_ptr<CFGEdge>>
            getOutEdgesOf(std::shared_ptr<ir::Stmt> stmt) const = 0;

        /**
         * @return the intermediate representation corresponding to this CFG
         */
        [[nodiscard]] virtual std::shared_ptr<ir::IR> getIR() const = 0;
    };

    class DefaultCFGEdge: public CFGEdge {
    public:

        [[nodiscard]] Kind getKind() const override;

        [[nodiscard]] std::shared_ptr<ir::Stmt> getSource() const override;

        [[nodiscard]] std::shared_ptr<ir::Stmt> getTarget() const override;

        /**
         * Construct a default CFG edge
         * @param source the source statement
         * @param target the target statement
         * @param kind the edge kind
         */
        DefaultCFGEdge(std::shared_ptr<ir::Stmt> source, std::shared_ptr<ir::Stmt> target,
                       Kind kind = Kind::UNKNOWN_EDGE);

    private:

        std::shared_ptr<ir::Stmt> source; ///< the source statement

        std::shared_ptr<ir::Stmt> target; ///< the target statement

        Kind kind; ///< the edge kind
    };

    class DefaultCFG final: public CFG {
    public:

        [[nodiscard]] std::shared_ptr<ir::Stmt> getEntry() const override;

        [[nodiscard]] std::shared_ptr<ir::Stmt> getExit() const override;

        [[nodiscard]] bool hasStmt(std::shared_ptr<ir::Stmt> stmt) const override;

        [[nodiscard]] bool
        hasEdge(std::shared_ptr<ir::Stmt> source, std::shared_ptr<ir::Stmt> target) const override;

        [[nodiscard]] std::unordered_set<std::shared_ptr<ir::Stmt>>
        getPredsOf(std::shared_ptr<ir::Stmt> stmt) const override;

        [[nodiscard]] std::unordered_set<std::shared_ptr<ir::Stmt>>
        getSuccsOf(std::shared_ptr<ir::Stmt> stmt) const override;

        [[nodiscard]] std::unordered_set<std::shared_ptr<CFGEdge>>
        getInEdgesOf(std::shared_ptr<ir::Stmt> stmt) const override;

        [[nodiscard]] std::unordered_set<std::shared_ptr<CFGEdge>>
        getOutEdgesOf(std::shared_ptr<ir::Stmt> stmt) const override;

        [[nodiscard]] std::shared_ptr<ir::IR> getIR() const override;

        void setIR(const std::shared_ptr<ir::IR>& myIR);

        void addEdge(const std::shared_ptr<CFGEdge>& edge);

        void setEntry(const std::shared_ptr<ir::Stmt>& entry);

        void setExit(const std::shared_ptr<ir::Stmt>& exit);

        explicit DefaultCFG();

    private:

        /// in edges of each statement
        std::unordered_map<std::shared_ptr<ir::Stmt>, std::unordered_set<std::shared_ptr<CFGEdge>>> inEdges;

        /// out edges of each statement
        std::unordered_map<std::shared_ptr<ir::Stmt>, std::unordered_set<std::shared_ptr<CFGEdge>>> outEdges;

        std::weak_ptr<ir::IR> myIR; ///< ir of this cfg

        std::shared_ptr<ir::Stmt> entry;

        std::shared_ptr<ir::Stmt> exit;

    };

} // graph

#endif //STATIC_ANALYZER_CFG_H
