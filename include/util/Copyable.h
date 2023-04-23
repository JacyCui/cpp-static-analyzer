#ifndef STATIC_ANALYZER_COPYABLE_H
#define STATIC_ANALYZER_COPYABLE_H

#include <memory>

namespace analyzer::util {

    /**
     * @class Copyable
     * @brief A copyable interface
     * @tparam T type of copy object
     */
    template <typename T>
    class Copyable {

        /**
         * @brief Creates and returns a copy of this object.
         * @return a copy of this object.
         */
        [[nodiscard]] virtual std::shared_ptr<T> copy() const = 0;

    };

} // util

#endif //STATIC_ANALYZER_COPYABLE_H
