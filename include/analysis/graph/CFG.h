#ifndef STATIC_ANALYZER_CFG_H
#define STATIC_ANALYZER_CFG_H

#include "util/Graph.h"
#include "ir/IR.h"

namespace analyzer::analysis::graph {

    template <typename N>
    class Edge: public util::AbstractEdge<N> {
    public:
        enum class Kind {
            ENTRY, ///< Edge from entry node to real start node.
            FALL_THROUGH, ///< Edge kind for fall-through to next statement.
            GOTO, ///< Edge kind for goto statements.
            IF_TRUE, ///< Edge kind for if statements when condition is true.
            IF_FALSE, ///< Edge kind for if statements when condition is false.
            SWITCH_CASE, ///< Edge kind for switch statements (explicit case).
            SWITCH_DEFAULT, ///< Edge kind for switch statements (default case).
            /**
             * Edge representing exceptional control flow from an
             * exception-raising node to an explicit handler for the exception.
             */
            CAUGHT_EXCEPTION,
            /**
             * Edge representing the possibility that a node raise an exception
             * that cannot be caught by the current method.
             * These edges always go to the exit node of the CFG.
             */
            UNCAUGHT_EXCEPTION,
            /**
             * Edge kind for return statements.
             * These edges always go to the exit node of the CFG.
             */
            RETURN
        };

        /**
         * @return the kind of the edge.
         */
        Kind getKind() {
            return kind;
        }

        Edge(Kind kind, std::shared_ptr<N> source, std::shared_ptr<N> target)
            : util::AbstractEdge<N>(source, target), kind(kind)
        {

        }

    private:

        Kind kind;

    };

    /**
     * Representation of a control-flow graph of a method.
     * @class CFG
     * @tparam N type of CFG nodes.
     */
    template <typename N>
    class CFG: public util::Graph<N> {
        /**
         * @return the IR of the method this CFG represents.
         */
        [[nodiscard]] virtual std::shared_ptr<ir::DefaultIR> getIR() const = 0;

        /**
         * @return the method this CFG represents.
         */
        [[nodiscard]] virtual std::shared_ptr<lang::CPPMethod> getMethod() const = 0;

        /**
         * @return the entry node of this CFG.
         */
        [[nodiscard]] virtual std::shared_ptr<N> getEntry() const = 0;

        /**
         * @return the exit node of this CFG
         */
        [[nodiscard]] virtual std::shared_ptr<N> getExit() const = 0;

        /**
         * @brief check the entry
         * @param node node to be checked
         * @return true if the given node is the entry of this CFG, otherwise false.
         */
        [[nodiscard]] virtual bool isEntry(std::shared_ptr<N> node) const = 0;

        /**
         * @brief check the exit
         * @param node node to be checked
         * @return true if the given node is the exit of this CFG, otherwise false.
         */
        [[nodiscard]] virtual bool isExit(std::shared_ptr<N> node) const = 0;

        /**
         * @param get the incoming edges
         * @param node node to get incoming edges
         * @return incoming edges of the given node.
         */
        [[nodiscard]] std::unordered_set<std::shared_ptr<Edge<N>>> getInEdgesOf(std::shared_ptr<N> node)
            const override = 0 ;

        /**
         * @brief get the outgoing edges
         * @param node node to get outgoing edges
         * @return outgoing edges of the given node.
         */
        [[nodiscard]] std::unordered_set<std::shared_ptr<Edge<N>>>
            getOutEdgesOf(std::shared_ptr<N> node) const override = 0;

    };



} // graph

#endif //STATIC_ANALYZER_CFG_H
