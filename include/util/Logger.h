#ifndef STATIC_ANALYZER_LOGGER_H
#define STATIC_ANALYZER_LOGGER_H

#include <llvm/Support/raw_ostream.h>

namespace analyzer::util {

    class Logger final {
    private:
        llvm::raw_ostream* os;
    public:
        /**
         * Construct a logger using given stream
         * @param os output stream to carry log information
         */
        Logger(llvm::raw_ostream* os);

        void setOutStream(llvm::raw_ostream* outStream)
        {
            os = outStream;
        }

        llvm::raw_ostream* getOutStream() const
        {
            return os;
        }

        void Progress(const std::string& str);

        void Warning(const std::string& str);

        void Info(const std::string& str);

        void Error(const std::string& str);

        void Debug(const std::string& str);

        void Success(const std::string& str);

    };

} // util

#endif //STATIC_ANALYZER_LOGGER_H
