#ifndef STATIC_ANALYZER_EXP_H
#define STATIC_ANALYZER_EXP_H

#include "language/Type.h"

namespace analyzer::language {
    class CPPMethod;
}

namespace lang = analyzer::language;

namespace analyzer::ir {

    /**
     * @class Exp
     * @brief Representations of expressions in ir
     */
    class Exp {

        [[nodiscard]] virtual std::shared_ptr<lang::Type> getType() const = 0;

    };

    class LValue: virtual public Exp {

    };

    class RValue: virtual public Exp {

    };


    /**
     * @class Var
     * @brief Representation of method/constructor parameters and local variables.
     */
    class Var final: public LValue, public RValue {
    public:

        [[nodiscard]] const lang::CPPMethod& getMethod() const;

        [[nodiscard]] const std::string& getName() const;

        [[nodiscard]] std::shared_ptr<lang::Type> getType() const override;

        Var(const lang::CPPMethod& method, std::string name, const std::shared_ptr<lang::Type>& type);

    private:

        const lang::CPPMethod& method;

        std::string name;

        std::shared_ptr<lang::Type> type;
    };


    class UnknownExp: public Exp {

    };

}

#endif //STATIC_ANALYZER_EXP_H
