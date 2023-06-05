#ifndef STATIC_ANALYZER_CONSTANTPROPAGATION_H
#define STATIC_ANALYZER_CONSTANTPROPAGATION_H

#include <memory>
#include <utility>

#include "analysis/dataflow/AnalysisDriver.h"
#include "analysis/dataflow/fact/MapFact.h"
#include "llvm/IR/Constants.h"

namespace analyzer::analysis::dataflow {

    /**
     * @class CPValue
     * @brief constant propagation value
     */
    class CPValue {
    public:

        /**
         * @return the Undefined value of constant propagation
         */
        [[nodiscard]] static std::shared_ptr<CPValue> getUndef();

        /**
         * @return the Not A Constant value of constant propagation
         */
        [[nodiscard]] static std::shared_ptr<CPValue> getNAC();

        /**
         * @brief make a constant value for constant propagation
         * @param constantValue the constant value
         * @return the constant value for constant propagation
         */
        [[nodiscard]] static std::shared_ptr<CPValue> makeConstant(const llvm::APSInt &constantValue);

    public:

        /**
         * @class CPValueType
         * @brief constant propagation value kind
         */
        enum class Kind {
            UNDEF, ///< undefined value for const propagation
            NAC, ///< Not A Constant value for const propagation
            CONSTANT ///< constant value for const propagation
        };

        /**
         * @brief constructor for constant propagation value
         * @param kind the constant propagation value type
         * @param constantValue the constant value
         */
        explicit CPValue(Kind kind, llvm::APSInt constantValue = llvm::APSInt());

        /**
         * @return true if the constant propagation value is undefined, false otherwise
         */
        [[nodiscard]] bool isUndef() const;

        /**
         * @return true if the constant propagation value is not a constant, false otherwise
         */
        [[nodiscard]] bool isNAC() const;

        /**
         * @return true if the constant propagation value is a constant, false otherwise
         */
        [[nodiscard]] bool isConstant() const;

        /**
         * @brief get the constant value
         * @return the constant value
         */
        [[nodiscard]] const llvm::APSInt& getConstantValue() const;

        /**
         * @brief operator== for constant propagation value
         * @param other the other constant propagation value
         * @return true if the constant propagation value is equal to the other constant propagation value, false otherwise
         */
        bool operator==(const CPValue& other) const {
            return kind == other.kind && constantValue == other.constantValue;
        }

    private:

        static std::shared_ptr<CPValue> Undef, NAC; ///< two special constant propagation value

    private:

        Kind kind; ///< constant propagation value kind

        llvm::APSInt constantValue; ///< constant value

    };

    /**
     * @class CPFact
     * @brief constant propagation fact
     */
    class CPFact: public fact::MapFact<ir::Var, CPValue> {
    public:

        /**
         * @brief get the CPValue of a given var
         * @param key the var to be searched
         * @return the CPValue to which the specified key is mapped,
         * or Undef if this map contains no mapping for the given var
         */
        [[nodiscard]] std::shared_ptr<CPValue> get(const std::shared_ptr<ir::Var>& key) const override;

        /**
         * @brief Updates the key-value mapping in this fact.
         * @param key the var to update
         * @param value the CPValue to be bound to the var
         * @return true if the update changes this fact, otherwise
         */
        bool update(const std::shared_ptr<ir::Var>& key, const std::shared_ptr<CPValue>& value) override;

    };


    /**
     * @class CPResult
     * @brief constant propagation result
     */
    class CPResult : public fact::DataflowResult<CPFact> {
    public:
    
        /**
         * @brief constructor for constant propagation result
         * @param exprValues the map from clang expr to constant propagation value
         */
        explicit CPResult(std::shared_ptr<std::unordered_map<const clang::Expr*, std::shared_ptr<CPValue>>> exprValues);

        /**
         * @brief get the constant propagation value of a given clang expr
         * @param expr the clang expr to be searched
         * @return the constant propagation value to which the specified clang expr is mapped,
         * or nullptr if this map contains no mapping for the given clang expr
         */
        [[nodiscard]] std::shared_ptr<CPValue> getExprValue(const clang::Expr* expr) const;

    private:
        std::shared_ptr<std::unordered_map<const clang::Expr*, std::shared_ptr<CPValue>>> exprValues;
    };

    /**
     * @class ConstantPropagation
     * @brief constant propagation analysis
     */
    class ConstantPropagation: public AnalysisDriver<CPFact> {
    public:

        /**
         * @brief constructor for constant propagation analysis
         * @param analysisConfig the analysis configuration
         */
        explicit ConstantPropagation(std::unique_ptr<config::AnalysisConfig>& analysisConfig);

    protected:

        [[nodiscard]] std::unique_ptr<DataflowAnalysis<CPFact>>
            makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const override;

    };

} // dataflow


#endif //STATIC_ANALYZER_CONSTANTPROPAGATION_H
