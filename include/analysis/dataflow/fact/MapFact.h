#ifndef STATIC_ANALYZER_MAPFACT_H
#define STATIC_ANALYZER_MAPFACT_H

#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "util/Copyable.h"

namespace analyzer::analysis::dataflow::fact {

    /**
     * @class MapFact
     * @brief Represents map-like data-flow facts.
     *
     * Elements are checked by identity rather than equality !!!
     *
     * @tparam K key type
     * @tparam V value type
     */
    template <typename K, typename V>
    class MapFact : public util::Copyable<MapFact<K, V>> {
    public:

        /**
         * @brief get the value of a given key
         * @param key the key to be searched
         * @return the value to which the specified key is mapped,
         * or nullptr if this map contains no mapping for the key.
         */
        [[nodiscard]] std::shared_ptr<V> get(const std::shared_ptr<K>& key) const
        {
            if (map.find(key) == map.end()) {
                return nullptr;
            }
            return map.at(key);
        }

        /**
         * @brief Updates the key-value mapping in this fact.
         * @param key the key to update
         * @param value the value to be bound to the key
         * @return true if the update changes this fact, otherwise
         */
        bool update(const std::shared_ptr<K>& key, const std::shared_ptr<V>& value)
        {
            if (map.find(key) == map.end()) {
                map.emplace(key, value);
                return true;
            }
            if (map.at(key) == value) {
                return false;
            }
            map.at(key) = value;
            return true;
        }

        /**
         * @brief Removes the key-value mapping for given key.
         * @param key the key to remove its value
         * @return the previous value associated with key, or null if there was no mapping for key.
         */
        std::shared_ptr<V> remove(const std::shared_ptr<K>& key)
        {
            if (map.find(key) == map.end()) {
                return nullptr;
            }
            std::shared_ptr<V> result = map.at(key);
            map.erase(key);
            return result;
        }

        /**
         * @brief Copies the content from given fact to this fact.
         * @param fact the fact to be copied
         * @return true if this fact changed as a result of the call, otherwise false.
         */
        bool copyFrom(const std::shared_ptr<MapFact<K, V>> fact)
        {
            bool changed = false;
            for (const auto& [key, value] : fact->getMap()) {
                changed = update(key, value) || changed;
            }
            return changed;
        }

        /**
         * @brief creates and returns a copy of this fact
         * @return a copy of this fact
         */
        [[nodiscard]] std::shared_ptr<MapFact<K, V>> copy() const override
        {
            return std::make_shared<MapFact<K, V>>(map);
        }

        /**
         * @brief Clears all content in this fact.
         */
        void clear()
        {
            map.clear();
        }

        /**
         * @return a set of the keys contained in this fact.
         */
        [[nodiscard]] std::unordered_set<std::shared_ptr<K>> keySet() const
        {
            std::unordered_set<std::shared_ptr<K>> result;
            for (auto& [k, _] : map) {
                result.emplace(k);
            }
            return result;
        }

        /**
         * @return a set of the values contained in this fact.
         */
        [[nodiscard]] std::unordered_set<std::shared_ptr<K>> valueSet() const
        {
            std::unordered_set<std::shared_ptr<K>> result;
            for (auto& [_, v] : map) {
                result.emplace(v);
            }
            return result;
        }

        /**
         * @return true if this fact is empty, otherwise false
         */
        [[nodiscard]] bool isEmpty() const
        {
            return map.empty();
        }

        /**
         * @return the size of this map fact
         */
        [[nodiscard]] std::size_t size() const
        {
            return map.size();
        }

        /**
         * @brief compares equality of map fact
         * @param other another map fact
         * @return true if this map fact is equal to other map fact, otherwise false
         */
        [[nodiscard]] bool equalsTo(const std::shared_ptr<MapFact<K, V>>& other) const
        {
            return map == other->getMap();
        }

        /**
         * @brief call processor function for each key-value pairs in this map fact
         * @param processor a processor function to process each key-value pair
         */
        void forEach(std::function<void(std::shared_ptr<K>, std::shared_ptr<V>)> processor)
        {
            for (auto [k, v] : map) {
                processor(k, v);
            }
        }

        /**
         * @brief Constructs a new MapFact with the same mappings as specified Map.
         * @param map the map whose mappings are to be placed in this map.
         */
        explicit MapFact(std::unordered_map<std::shared_ptr<K>, std::shared_ptr<V>> map)
            :map(std::move(map))
        {

        }

        /**
         * @brief Construct an empty set
         */
        MapFact()
            : MapFact(std::unordered_map<std::shared_ptr<K>, std::shared_ptr<V>>{})
        {

        }

    private:

        /**
         * @return a constant reference to the inner map
         */
        const std::unordered_map<std::shared_ptr<K>, std::shared_ptr<V>>& getMap() const
        {
            return map;
        }

        std::unordered_map<std::shared_ptr<K>, std::shared_ptr<V>>
            map; ///< The map holding the mappings of this MapFact.

    };

} // fact

#endif //STATIC_ANALYZER_MAPFACT_H
