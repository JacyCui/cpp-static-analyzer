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
         * @class CPValueType
         * @brief constant propagation value kind
         */
        enum class Kind {
            UNDEF,
            NAC,
            CONSTANT
        };

        /**
         * @brief constructor for constant propagation value
         * @param kind the constant propagation value type
         * @param constantValue the constant value
         */
        explicit CPValue(Kind kind, llvm::APSInt constantValue = llvm::APSInt())
            : kind(kind), constantValue(std::move(constantValue)) { }

        /**
         * @brief check if the constant propagation value is undefined
         * @return true if the constant propagation value is undefined, false otherwise
         */
        [[nodiscard]] bool isUndef() const {
            return kind == Kind::UNDEF;
        }

        /**
         * @brief check if the constant propagation value is not a constant
         * @return true if the constant propagation value is not a constant, false otherwise
         */
        [[nodiscard]] bool isNAC() const {
            return kind == Kind::NAC;
        }

        /**
         * @brief check if the constant propagation value is a constant
         * @return true if the constant propagation value is a constant, false otherwise
         */
        [[nodiscard]] bool isConstant() const {
            return kind == Kind::CONSTANT;
        }

        /**
         * @brief get the constant propagation Undefine value
         * @return the constant propagation Undefine value
         */
        [[nodiscard]] static std::shared_ptr<CPValue> getUndef() {
            return Undef;
        }

        /**
         * @brief get the constant propagation Not A Constant value
         * @return the constant propagation Not A Constant value 
         */
        [[nodiscard]] static std::shared_ptr<CPValue> getNAC() {
            return NAC;
        }

        /**
         * @brief make a constant propagation constant value
         * @param constantValue the constant value
         * @return the constant propagation constant value
         */
        [[nodiscard]] static std::shared_ptr<CPValue> makeConstant(const llvm::APSInt &constantValue) {
            return std::make_shared<CPValue>(Kind::CONSTANT, constantValue);
        }

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

    protected:

        Kind kind;
        llvm::APSInt constantValue;

        static std::shared_ptr<CPValue> Undef, NAC;

    };

    /**
     * @class CPFact
     * @brief constant propagation fact
     */
    class CPFact : public fact::MapFact<ir::Var, CPValue> {
    public:

        // CPFact(const fact::MapFact<ir::Var, CPValue>& mapFact)
        //     : MapFact(mapFact) { }

        /**
         * @brief get the CPValue of a given var
         * @param key the var to be searched
         * @return the CPValue to which the specified key is mapped,
         * or Undef if this map contains no mapping for the given var
         */
        [[nodiscard]] std::shared_ptr<CPValue> get(const std::shared_ptr<ir::Var>& key) const;

        /**
         * @brief Updates the key-value mapping in this fact.
         * @param key the var to update
         * @param value the CPValue to be bound to the var
         * @return true if the update changes this fact, otherwise
         */
        bool update(const std::shared_ptr<ir::Var>& key, const std::shared_ptr<CPValue>& value);

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
