#ifndef STATIC_ANALYZER_LOGGER_H
#define STATIC_ANALYZER_LOGGER_H

#include <string_view>

#include <llvm/Support/raw_ostream.h>

namespace analyzer::util {

    /**
     * @class Logger
     * @brief a logger class to encapsulate all kinds of information
     */
    class Logger final {
    private:
        enum class Color {
            RESET, BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
        };

        static std::unordered_map<Color, std::string_view> colors;

    private:
        llvm::raw_ostream* os; ///< an output stream
        bool enabled; ///< whether the logger is enabled

    public:
        /**
         * @brief Construct a logger using given stream
         * @param os output stream to carry log information
         * @param enabled whether this logger is enabled
         */
        Logger(llvm::raw_ostream* os, bool enabled = true);
        Logger(const Logger&) = default;
        Logger& operator=(const Logger&) = default;
        ~Logger() = default;

        /**
         * @brief set the output stream of the logger
         * @param outStream pointer to an output stream
         */
        void setOutStream(llvm::raw_ostream* outStream)
        {
            os = outStream;
        }

        /**
         * @brief get the current output stream of the logger
         * @return pointer to an output stream
         */
        llvm::raw_ostream* getOutStream() const
        {
            return os;
        }

        /**
         * @return whether this logger is enabled
         */
        bool isEnabled() const
        {
            return enabled;
        }

        /**
         * @brief enable this logger
         */
        void enable() {
            enabled = true;
        }

        /**
         * @brief disable this logger
         */
        void disable()
        {
            enabled = false;
        }

        /**
         * @brief log as progress (BLUE)
         * @param str log information
         */
        void Progress(const std::string& str);

        /**
         * @brief log as warning (YELLOW)
         * @param str log information
         */
        void Warning(const std::string& str);

        /**
         * @brief log as info (WHITE)
         * @param str log information
         */
        void Info(const std::string& str);

        /**
         * @brief log as error (RED)
         * @param str log information
         */
        void Error(const std::string& str);

        /**
         * @brief log as debug (CYAN)
         * @param str log information
         */
        void Debug(const std::string& str);

        /**
         * @brief log as success (GREEN)
         * @param str log information
         */
        void Success(const std::string& str);

    };

} // util

#endif //STATIC_ANALYZER_LOGGER_H
