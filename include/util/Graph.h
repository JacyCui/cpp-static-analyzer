#ifndef STATIC_ANALYZER_GRAPH_H
#define STATIC_ANALYZER_GRAPH_H

#include <unordered_set>

namespace analyzer::util {

    /**
     * @brief General interface for graph edges.
     * @class Edge
     * @tparam N type of nodes
     */
    template <typename N>
    class Edge {
    public:

        /**
         * @return the source node of the edge.
         */
        [[nodiscard]] virtual std::shared_ptr<N> getSource() const = 0;

        /**
         * @return the target node of the edge.
         */
        [[nodiscard]] virtual std::shared_ptr<N> getTarget() const = 0;
    };

    /**
     * @brief Abstract template for graph edges.
     * @class AbstractEdge
     * @tparam N type of nodes
     */
    template <typename N>
    class AbstractEdge: public Edge<N> {
    public:

        std::shared_ptr<N> getSource() const override {
            return source;
        }

        std::shared_ptr<N> getTarget() const override {
            return target;
        }

        AbstractEdge(std::shared_ptr<N>& source, std::shared_ptr<N>& target)
                :source(source), target(target)
        {

        }

    protected:
        std::shared_ptr<N> source;
        std::shared_ptr<N> target;
    };


    /**
     * @brief Representation of a directed graph.
     * @class Graph
     * @tparam N type of nodes
     */
    template <typename N>
    class Graph {
    public:

        /**
         * @brief check node existence
         * @param node node to be checked
         * @return true if this graph has given node, otherwise false.
         */
        [[nodiscard]] virtual bool hasNode(std::shared_ptr<N> node) const = 0;

        /**
         * @brief check edge existence
         * @param source source node
         * @param target target node
         * @return true if this graph has an edge from given source to target,
         * otherwise false.
         */
        [[nodiscard]] virtual bool hasEdge(std::shared_ptr<N> source, std::shared_ptr<N> target) const = 0;

        /**
         * @brief check edge existence
         * @param edge edge to be checked
         * @return true if this graph has the given edge, otherwise false.
         */
        [[nodiscard]] virtual bool hasEdge(std::shared_ptr<Edge<N>> edge) const {
            return hasEdge(edge->getSource(), edge->getTarget());
        }

        /**
         * @brief get the predecessors
         * @param node node to search predecessors
         * @return the predecessors of given node in this graph.
         */
        [[nodiscard]] virtual std::unordered_set<std::shared_ptr<N>> getPredsOf(std::shared_ptr<N> node) const = 0;

        /**
         * @brief get the successors
         * @param node node to search successors
         * @return the successors of given node in this graph.
         */
        [[nodiscard]] virtual std::unordered_set<std::shared_ptr<N>> getSuccsOf(std::shared_ptr<N> node) const = 0;

        /**
         * @brief get the incoming edges
         * @param node node to get incoming edges
         * @return incoming edges of the given node.
         */
        [[nodiscard]] virtual std::unordered_set<std::shared_ptr<Edge<N>>> getInEdgesOf(std::shared_ptr<N> node) const {
            std::unordered_set<std::shared_ptr<Edge<N>>> result;
            std::transform(getPredsOf(node).begin(), getPredsOf(node).end(),
                           result.begin(), [&](std::shared_ptr<N>& pred) -> std::shared_ptr<Edge<N>> {
                return std::make_shared<Edge<N>>(pred, node);
            });
            return result;
        }

        /**
         * @brief count in degrees
         * @param node node to count in degrees
         * @return the number of in edges of the given node.
         */
        [[nodiscard]] virtual std::size_t getInDegreeOf(std::shared_ptr<N> node) const {
            return getInEdgesOf(node).size();
        }

        /**
         * @brief get the outgoing edges
         * @param node node to get outgoing edges
         * @return outgoing edges of the given node.
         */
        [[nodiscard]] virtual std::unordered_set<std::shared_ptr<Edge<N>>> getOutEdgesOf(std::shared_ptr<N> node) const {
            std::unordered_set<std::shared_ptr<Edge<N>>> result;
            std::transform(getSuccsOf(node).begin(), getSuccsOf(node).end(),
                           result.begin(), [&](std::shared_ptr<N>& succ) -> std::shared_ptr<Edge<N>> {
                return std::make_shared<Edge<N>>(node, succ);
            });
            return result;
        }

        /**
         * @brief count out degrees
         * @param node node to count out degrees
         * @return the number of out edges of the given node.
         */
        [[nodiscard]] virtual std::size_t getOutDegreeOf(std::shared_ptr<N> node) const {
            return getOutEdgesOf(node).size();
        }

        /**
         * @return all nodes of this graph.
         */
        [[nodiscard]] virtual std::unordered_set<std::shared_ptr<N>> getNodes() const = 0;

        /**
         * @return the number of the nodes in this graph.
         */
        [[nodiscard]] virtual std::size_t getNumberOfNodes() const {
            return getNodes().size();
        }

    };

} // util

#endif //STATIC_ANALYZER_GRAPH_H
