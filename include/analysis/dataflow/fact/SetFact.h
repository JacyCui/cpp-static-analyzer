#ifndef STATIC_ANALYZER_SETFACT_H
#define STATIC_ANALYZER_SETFACT_H

#include <unordered_set>
#include <memory>
#include <functional>
#include <algorithm>

#include "util/Copyable.h"

namespace analyzer::analysis::dataflow::fact {

    /**
     * @class SetFact
     * @brief Represents set-like data-flow facts.
     *
     * Elements are checked by identity rather than equality !!!
     *
     * @tparam E elements type
     */
    template <typename E>
    class SetFact final: public util::Copyable<SetFact<E>> {
    public:

        /**
         * @brief check the existence of an element
         * @param e the element to be checked
         * @return true if this set contains the specified element, otherwise false.
         */
        [[nodiscard]] bool contains(const std::shared_ptr<E>& e) const
        {
            return set.find(e) != set.end();
        }

        /**
         * @brief Adds an element to this fact.
         * @param e the element to be added
         * @return true if this fact changed as a result of the call, otherwise false.
         */
        bool add(const std::shared_ptr<E>& e)
        {
            return set.emplace(e).second;
        }

        /**
         * @brief Removes an element from this fact
         * @param e the element to be removed
         * @return true if any elements were removed as a result of the call, otherwise false.
         */
        bool remove(const std::shared_ptr<E>& e)
        {
            return set.erase(e) == 1;
        }

        /**
         * @brief Removes all the elements of this fact that satisfy the given predicate.
         * @param filter a predicate for elements to be removed
         * @return true if any elements were removed as a result of the call, otherwise false.
         */
        bool removeIf(std::function<bool(const std::shared_ptr<E>&)> filter)
        {
            std::size_t oldSize = set.size();
            std::vector<std::shared_ptr<E>> toRemove;
            for (const std::shared_ptr<E>& e : set) {
                if (filter(e)) {
                    toRemove.emplace_back(e);
                }
            }
            for (const std::shared_ptr<E>& e : toRemove) {
                set.erase(e);
            }
            return set.size() != oldSize;
        }

        /**
         * @brief Removes all elements of other fact.
         * @param other another set fact
         * @return true if this fact changed as a result of the call, otherwise false.
         */
        bool removeAll(const std::shared_ptr<SetFact<E>>& other)
        {
            return removeIf([&](const std::shared_ptr<E>& e) -> bool {
               return other->contains(e);
            });
        }

        /**
         * @brief Unions other fact into this fact.
         * @param other another set fact
         * @return true if this fact changed as a result of the call, otherwise false.
         */
        bool unionN(const std::shared_ptr<SetFact<E>>& other)
        {
            std::size_t oldSize = set.size();
            set.insert(other->getSet().begin(), other->getSet().end());
            return set.size() != oldSize;
        }

        /**
         * @brief create a new fact as the union of this and other
         * @param other another set fact
         * @return a new fact which is the union of this and other facts.
         */
        [[nodiscard]] std::shared_ptr<SetFact<E>> unionWith(const std::shared_ptr<SetFact<E>>& other) const
        {
            std::unordered_set<std::shared_ptr<E>> result;
            std::set_union(set.begin(), set.end(),
                           other->getSet().begin(), other->getSet().end(),
                           std::inserter(result, result.begin()));
            return std::make_shared<SetFact<E>>(std::move(result));
        }

        /**
         * @brief Intersects this fact with other fact.
         * @param other another set fact
         * @return true if this fact changed as a result of the call, otherwise false.
         */
        bool intersect(const std::shared_ptr<SetFact<E>>& other)
        {
            return removeIf([&] (const std::shared_ptr<E>& e) -> bool {
                return !other->contains(e);
            });
        }

        /**
         * @brief create a new fact as the intersection of this and other
         * @param other another set fact
         * @return a new fact which is the intersection of this and other facts.
         */
        [[nodiscard]] std::shared_ptr<SetFact<E>> intersectWith(const std::shared_ptr<SetFact<E>>& other) const
        {
            std::unordered_set<std::shared_ptr<E>> result;
            std::set_intersection(set.begin(), set.end(),
                           other->getSet().begin(), other->getSet().end(),
                           std::inserter(result, result.begin()));
            return std::make_shared<SetFact<E>>(std::move(result));
        }

        /**
         * @brief sets the content of this set to the same as other set.
         * @param other another set fact
         */
        void setSet(const std::shared_ptr<SetFact<E>>& other)
        {
            set = other->getSet();
        }

        /**
         * @return creates and returns a copy of this fact.
         */
        [[nodiscard]] std::shared_ptr<SetFact<E>> copy() const override
        {
            return std::make_shared<SetFact<E>>(set);
        }

        /**
         * @brief clears all content in this fact.
         */
        void clear()
        {
            set.clear();
        }

        /**
         * @return true if this fact is empty, otherwise false
         */
        [[nodiscard]] bool isEmpty() const
        {
            return set.empty();
        }

        /**
         * @return the size of this set fact
         */
        [[nodiscard]] std::size_t size() const
        {
            return set.size();
        }

        /**
         * @brief compares equality of set fact
         * @param other another set fact
         * @return true if this set fact is equal to other set fact, otherwise false
         */
        [[nodiscard]] bool equalsTo(const std::shared_ptr<SetFact<E>>& other) const
        {
            return set == other->getSet();
        }

        /**
         * @brief Construct a set from a given set
         * @param set
         */
        explicit SetFact(std::unordered_set<std::shared_ptr<E>> set)
            :set(std::move(set))
        {

        }

        /**
         * @brief Construct an empty set
         */
        SetFact()
            : SetFact(std::unordered_set<std::shared_ptr<E>>{})
        {

        }

    private:

        /**
         * @return a const reference to the inner set
         */
        const std::unordered_set<std::shared_ptr<E>>& getSet() const
        {
            return set;
        }

        std::unordered_set<std::shared_ptr<E>> set; ///< the inner set of this dataflow fact

    };

} // fact


#endif //STATIC_ANALYZER_SETFACT_H
