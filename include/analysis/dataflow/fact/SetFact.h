#ifndef STATIC_ANALYZER_SETFACT_H
#define STATIC_ANALYZER_SETFACT_H

#include <unordered_set>
#include <memory>

namespace analyzer::analysis::dataflow::fact {

    /**
     * @class SetFact
     * @brief Represents set-like data-flow facts.
     * @tparam E elements type
     */
    template <typename E>
    class SetFact final {
    public:

        /**
         * @brief check the existence of an element
         * @param e the element to be checked
         * @return true if this set contains the specified element, otherwise false.
         */
        [[nodiscard]] bool contains(std::shared_ptr<E> e) const
        {
            return set.find(e) != set.end();
        }

        /**
         * @brief Adds an element to this fact.
         * @param e the element to be added
         * @return true if this fact changed as a result of the call, otherwise false.
         */
        bool add(std::shared_ptr<E> e)
        {
            return set.emplace(e).second;
        }

        /**
         * @brief Removes an element from this fact
         * @param e the element to be removed
         * @return true if any elements were removed as a result of the call,
         * otherwise false.
         */
        bool remove(std::shared_ptr<E> e)
        {
            return set.erase(e) == 1;
        }

        /**
         * @brief Construct a set from a given set
         * @param set
         */
        explicit SetFact(std::unordered_set<std::shared_ptr<E>> set)
            :set(set)
        {

        }

        /**
         * @brief
         */
        SetFact()
            : SetFact({})
        {

        }

    private:

        std::unordered_set<std::shared_ptr<E>> set; ///< the inner set of this dataflow fact

    };

} // dataflow


#endif //STATIC_ANALYZER_SETFACT_H
