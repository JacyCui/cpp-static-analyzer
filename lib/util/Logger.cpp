//
// Created by 崔家才 on 2023/4/12.
//

#include "util/Logger.h"

namespace analyzer::util {

    Logger::Logger(llvm::raw_ostream* os)
        :os(os)
    {

    }

    void Logger::Progress(const std::string& str)
    {
        os->changeColor(llvm::raw_ostream::BLUE);
        *os << "[ Progress ] " << str << "\n";
        os->changeColor(llvm::raw_ostream::RESET);
    }

    void Logger::Warning(const std::string& str)
    {
        os->changeColor(llvm::raw_ostream::YELLOW);
        *os << "[ Warning ] " << str << "\n";
        os->changeColor(llvm::raw_ostream::RESET);
    }

    void Logger::Info(const std::string& str)
    {
        *os << "[ Info ] " << str << "\n";
    }

    void Logger::Error(const std::string& str)
    {
        os->changeColor(llvm::raw_ostream::RED);
        *os << "[ Error ] " << str << "\n";
        os->changeColor(llvm::raw_ostream::RESET);
    }

    void Logger::Debug(const std::string& str)
    {
        os->changeColor(llvm::raw_ostream::CYAN);
        *os << "[ Debug ] " << str << "\n";
        os->changeColor(llvm::raw_ostream::RESET);
    }

    void Logger::Success(const std::string& str)
    {
        os->changeColor(llvm::raw_ostream::GREEN);
        *os << "[ Success ] " << str << "\n";
        os->changeColor(llvm::raw_ostream::RESET);
    }

} // util
