#include "util/Logger.h"

namespace analyzer::util {

    std::unordered_map<Logger::Color, std::string_view> Logger::colors = {
            {Color::RESET, "\033[0m"},
            {Color::BLACK, "\033[30m"},
            {Color::RED, "\033[31m"},
            {Color::GREEN, "\033[32m"},
            {Color::YELLOW, "\033[33m"},
            {Color::BLUE, "\033[34m"},
            {Color::MAGENTA, "\033[35m"},
            {Color::CYAN, "\033[36m"},
            {Color::WHITE, "\033[37m"}
    };

    Logger::Logger(llvm::raw_ostream* os, bool enabled)
        :os(os), enabled(enabled)
    {

    }

    void Logger::setOutStream(llvm::raw_ostream* outStream)
    {
        os = outStream;
    }

    llvm::raw_ostream* Logger::getOutStream() const
    {
        return os;
    }

    bool Logger::isEnabled() const
    {
        return enabled;
    }

    void Logger::enable()
    {
        enabled = true;
    }

    void Logger::disable()
    {
        enabled = false;
    }

    void Logger::Progress(const std::string& str)
    {
        if (!enabled) {
            return;
        }
        *os << colors.at(Color::CYAN) << "[ Progress ] " << str
            << colors.at(Color::RESET) << "\n";
    }

    void Logger::Warning(const std::string& str)
    {
        if (!enabled) {
            return;
        }
        *os << colors.at(Color::YELLOW) << "[ Warning ] " << str
            << colors.at(Color::RESET) << "\n";
    }

    void Logger::Info(const std::string& str)
    {
        if (!enabled) {
            return;
        }
        *os << "[ Info ] " << str << "\n";
    }

    void Logger::Error(const std::string& str)
    {
        if (!enabled) {
            return;
        }
        *os << colors.at(Color::RED) << "[ Error ] " << str
            << colors.at(Color::RESET) << "\n";
    }

    void Logger::Debug(const std::string& str)
    {
        if (!enabled) {
            return;
        }
        *os << colors.at(Color::BLUE) << "[ Debug ] " << str
            << colors.at(Color::RESET) << "\n";
    }

    void Logger::Success(const std::string& str)
    {
        if (!enabled) {
            return;
        }
        *os << colors.at(Color::GREEN) << "[ Success ] " << str
            << colors.at(Color::RESET) << "\n";
    }

} // util
