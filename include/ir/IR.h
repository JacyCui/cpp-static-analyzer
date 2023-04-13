#ifndef STATIC_ANALYZER_IR_H
#define STATIC_ANALYZER_IR_H

#include <memory>

#include "language/CPPMethod.h"

namespace lang = analyzer::language;

namespace analyzer::ir {

    class IR {
    private:
        const std::shared_ptr<lang::CPPMethod> method; ///< the pointer this ir is representing

    };

} // ir

#endif //STATIC_ANALYZER_IR_H
