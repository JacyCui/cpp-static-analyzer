#include "analysis/dataflow/ConstantPropagation.h"

namespace analyzer::analysis::dataflow {

    //// ============== CPValue ============== ////

    std::shared_ptr<CPValue> CPValue::Undef = std::make_shared<CPValue>(Kind::UNDEF);

    std::shared_ptr<CPValue> CPValue::NAC = std::make_shared<CPValue>(Kind::NAC);

    CPValue::CPValue(Kind kind, llvm::APSInt constantValue)
        : kind(kind), constantValue(std::move(constantValue))
    {

    }

    bool CPValue::isUndef() const
    {
        return kind == Kind::UNDEF;
    }

    bool CPValue::isNAC() const
    {
        return kind == Kind::NAC;
    }

    bool CPValue::isConstant() const
    {
        return kind == Kind::CONSTANT;
    }

    std::shared_ptr<CPValue> CPValue::getUndef()
    {
        return Undef;
    }

    std::shared_ptr<CPValue> CPValue::getNAC()
    {
        return NAC;
    }

    std::shared_ptr<CPValue> CPValue::makeConstant(const llvm::APSInt &constantValue)
    {
        return std::make_shared<CPValue>(Kind::CONSTANT, constantValue);
    }

    const llvm::APSInt& CPValue::getConstantValue() const
    {
        if (!isConstant()) {
            throw std::runtime_error("CPValue is not a constant");
        }
        return constantValue;
    }

    //// ============== CPFact ============== ////

    std::shared_ptr<CPValue> CPFact::get(const std::shared_ptr<ir::Var>& key) const
    {
        std::shared_ptr<CPValue> value = fact::MapFact<ir::Var, CPValue>::get(key);
        if (value) {
            return value;
        }
        return CPValue::getUndef();
    }

    bool CPFact::update(const std::shared_ptr<ir::Var>& key, const std::shared_ptr<CPValue>& value)
    {
        if (value->isUndef()) {
            return remove(key) != nullptr;
        } else {
            return fact::MapFact<ir::Var, CPValue>::update(key, value);
        }
    }

    //// ============== CPResult ============== ////

    CPResult::CPResult() = default;

    void CPResult::updateExprValue(const clang::Expr* expr, const std::shared_ptr<CPValue>& value)
    {
        exprValues.insert_or_assign(expr, value);
    }

    std::shared_ptr<CPValue> CPResult::getExprValue(const clang::Expr* expr) const
    {
        if (exprValues.find(expr) != exprValues.end()) {
            return exprValues.at(expr);
        }
        return nullptr;
    }

    //// ============== ConstantPropagation ============== ////

    ConstantPropagation::ConstantPropagation(std::unique_ptr<config::AnalysisConfig> &analysisConfig)
        : AnalysisDriver<CPFact>(analysisConfig)
    {
    
    }

    std::unique_ptr<DataflowAnalysis<CPFact>>
        ConstantPropagation::makeAnalysis(const std::shared_ptr<graph::CFG> &cfg) const 
    {
        class Analysis : public AbstractDataflowAnalysis<CPFact> {
        public:

            [[nodiscard]] bool isForward() const override
            {
                return true;
            }

            [[nodiscard]] std::shared_ptr<CPFact> newBoundaryFact() const override
            {
                std::shared_ptr<CPFact> fact = std::make_shared<CPFact>();
                for (const std::shared_ptr<ir::Var>& param : cfg->getIR()->getParams()) {
                    if (checkVarType(param)) {
                        fact->update(param, CPValue::getNAC());
                    }
                }
                return fact;
            }

            [[nodiscard]] std::shared_ptr<CPFact> newInitialFact() const override
            {
                return std::make_shared<CPFact>();
            }

            void meetInto(std::shared_ptr<CPFact> fact,
                          std::shared_ptr<CPFact> target) const override
            {
                fact->forEach([&](const std::shared_ptr<ir::Var>& var,
                        const std::shared_ptr<CPValue>& value)
                {
                    std::shared_ptr<CPValue> targetValue = target->get(var);
                    if (value->isConstant()) {
                        if (targetValue->isUndef()) {
                            target->update(var, value);
                        } else if (targetValue->isConstant()
                                && targetValue->getConstantValue() != value->getConstantValue()) {
                            target->update(var, CPValue::getNAC());
                        }
                    } else if (value->isNAC() && !targetValue->isNAC()) {
                        target->update(var, CPValue::getNAC());
                    }
                });
            }

            [[nodiscard]] bool transferNode(
                    std::shared_ptr<ir::Stmt> stmt,
                    std::shared_ptr<CPFact> in,
                    std::shared_ptr<CPFact> out) const override
            {
                std::shared_ptr<fact::MapFact<ir::Var, CPValue>> oldOut = out->copy();
                out->copyFrom(in);
                const clang::Stmt* clangStmt = stmt->getClangStmt();
                if (clangStmt != nullptr) {
                    if (auto* DeclStmt = llvm::dyn_cast<clang::DeclStmt>(clangStmt))
                        for (clang::Decl* decl : DeclStmt->decls()) {
                            if (auto *varDecl = llvm::dyn_cast<clang::VarDecl>(decl))
                                if (checkClangVarDeclType(varDecl)) {
                                    if (varDecl->hasInit()) {
                                        out->update(mapVars.at(varDecl), calculateAndUpdateExprCPValue(varDecl->getInit(), in, out));
                                    }
                                }
                    } else if (auto* expr = llvm::dyn_cast<clang::Expr>(clangStmt)) {
                        calculateAndUpdateExprCPValue(expr, in, out);
                    }
                }
                return !out->equalsTo(oldOut);
            }

            explicit Analysis(const std::shared_ptr<graph::CFG>& myCFG)
                : AbstractDataflowAnalysis<CPFact>(myCFG), result(std::make_shared<CPResult>())
            {
                for (const std::shared_ptr<ir::Var>& var : myCFG->getIR()->getVars()) {
                    const clang::VarDecl* varDecl = var->getClangVarDecl();
                    if (varDecl != nullptr && checkClangVarDeclType(varDecl)) {
                        mapVars.insert_or_assign(varDecl, var);
                    }
                }
            }

        private:

            std::shared_ptr<CPResult> result;

            std::unordered_map<const clang::VarDecl *, std::shared_ptr<ir::Var>> mapVars;

            static bool checkClangVarDeclType(const clang::VarDecl *varDecl)
            {
                return varDecl->getType()->isIntegerType();
            }

            static bool checkVarType(const std::shared_ptr<ir::Var>& var)
            {
                return checkClangVarDeclType(var->getClangVarDecl());
            }

            std::shared_ptr<ir::Var> getVarFromExpr(const clang::Expr *expr) const
            {
                if (auto *declRef = llvm::dyn_cast<clang::DeclRefExpr>(expr)) {
                    if (auto *varDecl = llvm::dyn_cast<clang::VarDecl>(declRef->getDecl())) {
                        if (checkClangVarDeclType(varDecl)) {
                            return mapVars.at(varDecl);
                        }
                    }
                }
                return nullptr;
            }

            static bool OpcodeIsAssign(clang::BinaryOperatorKind opcode)
            {
                switch (opcode) {
                    case clang::BO_Assign:
                    case clang::BO_AddAssign:
                    case clang::BO_SubAssign:
                    case clang::BO_MulAssign:
                    case clang::BO_DivAssign:
                    case clang::BO_RemAssign:
                    case clang::BO_AndAssign:
                    case clang::BO_XorAssign:
                    case clang::BO_OrAssign:
                    case clang::BO_ShlAssign:
                    case clang::BO_ShrAssign:
                        return true;
                    default:
                        return false;
                }
            }

            std::shared_ptr<CPValue> calculateAndUpdateExprCPValue(
                    const clang::Expr *expr,
                    const std::shared_ptr<CPFact>& inFact,
                    const std::shared_ptr<CPFact>& outFact) const 
            {
                std::shared_ptr<CPValue> val = nullptr;
                
                if (auto* intLiteral = llvm::dyn_cast<clang::IntegerLiteral>(expr)) {
                    bool isUnsigned = !expr->getType()->isSignedIntegerType();
                    val = CPValue::makeConstant(llvm::APSInt(intLiteral->getValue(), isUnsigned));
                } else if (auto* characterLiteral = llvm::dyn_cast<clang::CharacterLiteral>(expr)) {
                    val = CPValue::makeConstant(llvm::APSInt(llvm::APInt(32, characterLiteral->getValue()), false));
                } else if (auto* castExpr = llvm::dyn_cast<clang::CastExpr>(expr)) {
                    std::shared_ptr<CPValue> subExprValue =
                            calculateAndUpdateExprCPValue(castExpr->getSubExpr(), inFact, outFact);
                    switch (castExpr->getCastKind()) {
                        case clang::CastKind::CK_LValueToRValue:
                            val = subExprValue;
                            break;
                        case clang::CastKind::CK_IntegralCast: 
                        case clang::CastKind::CK_NoOp:
                        {
                            if (subExprValue->isConstant()) {
                                if (auto* builtinTypeExpr = expr->getType()->getAs<clang::BuiltinType>()) {
                                    unsigned numBits = -1;
                                    bool isSigned = expr->getType()->isSignedIntegerType();
                                    switch (builtinTypeExpr->getKind()) {
                                        case clang::BuiltinType::Kind::Bool:
                                            numBits = 1;
                                            break;
                                        case clang::BuiltinType::Kind::Char_U:
                                        case clang::BuiltinType::Kind::UChar:
                                        case clang::BuiltinType::Kind::Char_S:
                                        case clang::BuiltinType::Kind::SChar:
                                            numBits = 8;
                                            break;
                                        case clang::BuiltinType::Kind::Char16:
                                        case clang::BuiltinType::Kind::UShort:
                                        case clang::BuiltinType::Kind::Short:
                                            numBits = 16;
                                            break;
                                        case clang::BuiltinType::Kind::Char32:
                                        case clang::BuiltinType::Kind::UInt:
                                        case clang::BuiltinType::Kind::Int:
                                            numBits = 32;
                                            break;
                                        case clang::BuiltinType::Kind::ULong:
                                        case clang::BuiltinType::Kind::Long:
                                        case clang::BuiltinType::Kind::ULongLong:
                                        case clang::BuiltinType::Kind::LongLong:
                                            numBits = 64;
                                            break;
                                        default:
                                            val = CPValue::getNAC();
                                    }
                                    if (val == nullptr) {
                                        auto constantValue = subExprValue->getConstantValue();
                                        uint64_t extValue = isSigned ? constantValue.getSExtValue() 
                                                            : constantValue.getZExtValue();
                                        val = CPValue::makeConstant(
                                                llvm::APSInt(llvm::APInt(numBits, extValue, isSigned), !isSigned));
                                    }
                                }
                            } else  {
                                val = subExprValue;
                            }
                            break;
                        }
                        default:
                            val = CPValue::getNAC();
                    }
                } else if (auto* declRef = llvm::dyn_cast<clang::DeclRefExpr>(expr)) {
                    if (auto* varDecl = llvm::dyn_cast<clang::VarDecl>(declRef->getDecl())) {
                        if (checkClangVarDeclType(varDecl)) {
                            val = inFact->get(mapVars.at(varDecl));
                        }
                    }
                    if (val == nullptr) {
                        val = CPValue::getNAC();
                    }
                } else if (auto* parenExpr = llvm::dyn_cast<clang::ParenExpr>(expr)) {
                    val = calculateAndUpdateExprCPValue(parenExpr->getSubExpr(), inFact, outFact);
                } else if (auto* unaryOp = llvm::dyn_cast<clang::UnaryOperator>(expr)) {
                    clang::Expr* subExpr = unaryOp->getSubExpr();
                    switch (unaryOp->getOpcode()) {
                        case clang::UnaryOperatorKind::UO_Plus:
                            val = calculateAndUpdateExprCPValue(subExpr, inFact, outFact);
                            break;
                        case clang::UnaryOperatorKind::UO_Minus: {
                            std::shared_ptr<CPValue> subExprValue =
                                    calculateAndUpdateExprCPValue(subExpr, inFact, outFact);
                            if (subExprValue->isConstant()) {
                                val = CPValue::makeConstant(-subExprValue->getConstantValue());
                            } else {
                                val = subExprValue;
                            }
                            break;
                        }
                        default:
                            if (unaryOp->isIncrementDecrementOp()) {
                                std::shared_ptr<CPValue> subExprValue =
                                        calculateAndUpdateExprCPValue(subExpr, inFact, outFact);
                                std::shared_ptr<CPValue> exprDecOrIncValue;
                                if (subExprValue->isConstant()) {
                                    auto constantValue = subExprValue->getConstantValue();
                                    auto newValue = llvm::APSInt(constantValue);
                                    if (unaryOp->isIncrementOp()) {
                                        exprDecOrIncValue = CPValue::makeConstant(++newValue);
                                    } else {
                                        exprDecOrIncValue = CPValue::makeConstant(--newValue);
                                    }
                                } else {
                                    exprDecOrIncValue = subExprValue;
                                }
                                if (auto var = getVarFromExpr(subExpr)) {
                                    outFact->update(var, exprDecOrIncValue);
                                }
                                if(unaryOp->isPostfix()) {
                                    val = subExprValue;
                                } else {
                                    val = exprDecOrIncValue;
                                }
                            } else {
                                val = CPValue::getNAC();
                            }
                    }
                } else if (auto* binaryOperator = llvm::dyn_cast<clang::BinaryOperator>(expr)) {
                    clang::Expr* lhs = binaryOperator->getLHS();
                    std::shared_ptr<CPValue> lhsValue =
                            calculateAndUpdateExprCPValue(lhs, inFact, outFact);
                    std::shared_ptr<CPValue> rhsValue =
                            calculateAndUpdateExprCPValue(binaryOperator->getRHS(), inFact, outFact);
                    if (binaryOperator->getOpcode() == clang::BinaryOperatorKind::BO_Assign) {
                        if (auto var = getVarFromExpr(lhs)) {
                            outFact->update(var, rhsValue);
                        }
                        val = rhsValue;
                    } else {
                        if (lhsValue->isNAC() || rhsValue->isNAC()) {
                            switch (binaryOperator->getOpcode()) {
                                case clang::BinaryOperatorKind::BO_Div:
                                case clang::BinaryOperatorKind::BO_DivAssign:
                                case clang::BinaryOperatorKind::BO_Rem:
                                case clang::BinaryOperatorKind::BO_RemAssign:
                                    if(rhsValue->isConstant() && rhsValue->getConstantValue().isZero()) {
                                        val = CPValue::getUndef();
                                    } else {
                                        val = CPValue::getNAC();
                                    }
                                    break;
                                default:
                                    val = CPValue::getNAC();
                            }
                        } else if (lhsValue->isConstant() && rhsValue->isConstant()) {
                            llvm::APSInt lhsConstant = lhsValue->getConstantValue();
                            llvm::APSInt rhsConstant = rhsValue->getConstantValue();
                            switch (binaryOperator->getOpcode()) {
                                case clang::BinaryOperatorKind::BO_Add:
                                case clang::BinaryOperatorKind::BO_AddAssign:
                                    val = CPValue::makeConstant(lhsConstant + rhsConstant);
                                    break;
                                case clang::BinaryOperatorKind::BO_Sub:
                                case clang::BinaryOperatorKind::BO_SubAssign:
                                    val = CPValue::makeConstant(lhsConstant - rhsConstant);
                                    break;
                                case clang::BinaryOperatorKind::BO_Mul:
                                case clang::BinaryOperatorKind::BO_MulAssign:
                                    val = CPValue::makeConstant(lhsConstant * rhsConstant);
                                    break;
                                case clang::BinaryOperatorKind::BO_Div:
                                case clang::BinaryOperatorKind::BO_DivAssign:
                                    if (rhsConstant.isZero()) {
                                        val = CPValue::getUndef();
                                    } else {
                                        val = CPValue::makeConstant(lhsConstant / rhsConstant);
                                    }
                                    break;
                                case clang::BinaryOperatorKind::BO_Rem:
                                case clang::BinaryOperatorKind::BO_RemAssign:
                                    if (rhsConstant.isZero()) {
                                        val = CPValue::getUndef();
                                    } else {
                                        val = CPValue::makeConstant(lhsConstant % rhsConstant);
                                    }
                                    break;
                                case clang::BinaryOperatorKind::BO_And:
                                case clang::BinaryOperatorKind::BO_AndAssign:
                                    val = CPValue::makeConstant(lhsConstant & rhsConstant);
                                    break;
                                case clang::BinaryOperatorKind::BO_Or:
                                case clang::BinaryOperatorKind::BO_OrAssign:
                                    val = CPValue::makeConstant(lhsConstant | rhsConstant);
                                    break;
                                case clang::BinaryOperatorKind::BO_Xor:
                                case clang::BinaryOperatorKind::BO_XorAssign:
                                    val = CPValue::makeConstant(lhsConstant ^ rhsConstant);
                                    break;
                                case clang::BinaryOperatorKind::BO_Shl:
                                case clang::BinaryOperatorKind::BO_ShlAssign:
                                    val = CPValue::makeConstant(lhsConstant << rhsConstant.getLimitedValue());
                                    break;
                                case clang::BinaryOperatorKind::BO_Shr:
                                case clang::BinaryOperatorKind::BO_ShrAssign:
                                    val = CPValue::makeConstant(lhsConstant >> rhsConstant.getLimitedValue());
                                    break;
                                default:
                                    val = CPValue::getNAC();
                                    break;
                            }
                        } else {
                            val = CPValue::getUndef();
                        }
                        if (OpcodeIsAssign(binaryOperator->getOpcode())) {
                            if (auto var = getVarFromExpr(lhs)) {
                                outFact->update(var, val);
                            }
                        }
                    }
                } else if (auto *arraySubscriptExpr = llvm::dyn_cast<clang::ArraySubscriptExpr>(expr)) {
                    calculateAndUpdateExprCPValue(arraySubscriptExpr->getBase(), inFact, outFact);
                    calculateAndUpdateExprCPValue(arraySubscriptExpr->getIdx(), inFact, outFact);
                    val = CPValue::getNAC();
                } else if (auto* conditionalOperator = llvm::dyn_cast<clang::ConditionalOperator>(expr)) {
                    calculateAndUpdateExprCPValue(conditionalOperator->getCond(), inFact, outFact);
                    calculateAndUpdateExprCPValue(conditionalOperator->getTrueExpr(), inFact, outFact);
                    calculateAndUpdateExprCPValue(conditionalOperator->getFalseExpr(), inFact, outFact);
                    val = CPValue::getNAC();
                } else if(auto* callExpr = llvm::dyn_cast<clang::CallExpr>(expr)) {
                    calculateAndUpdateExprCPValue(callExpr->getCallee(), inFact, outFact);
                    for(const clang::Expr* arg : callExpr->arguments()) {
                        calculateAndUpdateExprCPValue(arg, inFact, outFact);
                    }
                    val = CPValue::getNAC();
                } else {
                    val = CPValue::getNAC();
                }
                
                result->updateExprValue(expr, val);
                return val;
            }

            [[nodiscard]] std::shared_ptr<fact::DataflowResult<CPFact>>
                getResult() const override
            {
                return result;
            }

        };

        return std::make_unique<Analysis>(cfg);
    }

}
