#include "analysis/dataflow/ConstantPropagation.h"

namespace analyzer::analysis::dataflow {

    //// ============== CPValue ============== ////

    std::shared_ptr<CPValue> CPValue::Undef = std::make_shared<CPValue>(Kind::UNDEF);
    std::shared_ptr<CPValue> CPValue::NAC = std::make_shared<CPValue>(Kind::NAC);

    [[nodiscard]] const llvm::APSInt& CPValue::getConstantValue() const {
        if (!isConstant()) {
            throw std::runtime_error("CPValue is not a constant");
        }
        return constantValue;
    }

    //// ============== CPFact ============== ////

    [[nodiscard]] std::shared_ptr<CPValue> CPFact::get(const std::shared_ptr<ir::Var>& var) const {
        std::shared_ptr<CPValue> value = fact::MapFact<ir::Var, CPValue>::get(var);
        if (value == nullptr) {
            return CPValue::getUndef();
        }
        return value;
    }

    bool CPFact::update(const std::shared_ptr<ir::Var>& var, const std::shared_ptr<CPValue>& value) {
        if (value->isUndef()) {
            return remove(var) != nullptr;
        } else {
            return fact::MapFact<ir::Var, CPValue>::update(var, value);
        }
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
                auto params = cfg->getIR()->getParams();
                for (const std::shared_ptr<ir::Var>& param : params) {
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
                fact->forEach([&](const std::shared_ptr<ir::Var>& var, const std::shared_ptr<CPValue>& value) {
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
                auto clangStmt = stmt->getClangStmt();
                if (clangStmt != nullptr) {
                    if (auto *DeclStmt = llvm::dyn_cast<clang::DeclStmt>(clangStmt))
                        for (auto decl : DeclStmt->decls()) {
                            if (auto *varDecl = llvm::dyn_cast<clang::VarDecl>(decl))
                                if (checkClangVarDeclType(varDecl)) {
                                    std::shared_ptr<ir::Var> var = mapVars.at(varDecl);
                                    if (varDecl->hasInit()) {
                                        auto initValue = calculateExprCPValue(varDecl->getInit(), in);
                                        out->update(var, initValue);
                                    }
                                }
                    } else if(auto* binaryOp = llvm::dyn_cast<clang::BinaryOperator>(clangStmt)) {
                        switch (binaryOp->getOpcode()) {
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
                            {
                                auto lhs = binaryOp->getLHS();
                                if (auto *lhsDeclRef = llvm::dyn_cast<clang::DeclRefExpr>(lhs))
                                    if (auto *lhsVarDecl = llvm::dyn_cast<clang::VarDecl>(lhsDeclRef->getDecl()))
                                        if (checkClangVarDeclType(lhsVarDecl)) {
                                            std::shared_ptr<ir::Var> lhsVar = mapVars.at(lhsVarDecl);
                                            auto exprValue = calculateExprCPValue(binaryOp, in);
                                            out->update(lhsVar, exprValue);
                                        }
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
                return !out->equalsTo(oldOut);
            }

            explicit Analysis(const std::shared_ptr<graph::CFG>& myCFG)
                : AbstractDataflowAnalysis<CPFact>(myCFG)
            {
                auto vars = myCFG->getIR()->getVars();
                for (const std::shared_ptr<ir::Var>& var : vars) {
                    auto* varDecl = var->getClangVarDecl();
                    if (varDecl != nullptr && checkVarType(var)) {
                        mapVars[varDecl] = var;
                    }
                }
            }
        private:
            std::unordered_map<const clang::VarDecl *, std::shared_ptr<ir::Var>> mapVars;

            static bool checkClangVarDeclType(const clang::VarDecl *varDecl) {
                return varDecl->getType()->isIntegerType();
            }

            static bool checkVarType(const std::shared_ptr<ir::Var>& var) {
                return checkClangVarDeclType(var->getClangVarDecl());
            }

            std::shared_ptr<CPValue> calculateExprCPValue(
                    const clang::Expr *expr,
                    const std::shared_ptr<CPFact>& inFact) const 
            {
                if (!expr->getType()->isIntegerType()) {
                    return CPValue::getNAC();
                } else if (auto* intLiteral = llvm::dyn_cast<clang::IntegerLiteral>(expr)) {
                    bool isUnsigned = !expr->getType()->isSignedIntegerType();
                    return CPValue::makeConstant(llvm::APSInt(intLiteral->getValue(), isUnsigned));
                } else if(auto* castExpr = llvm::dyn_cast<clang::CastExpr>(expr)) {
                    clang::CastKind castKind = castExpr->getCastKind();
                    auto subExpr = castExpr->getSubExpr();
                    switch (castKind) {
                        case clang::CastKind::CK_LValueToRValue:
                            return calculateExprCPValue(subExpr, inFact);
                        case clang::CastKind::CK_IntegralCast: {
                            auto subExprValue = calculateExprCPValue(subExpr, inFact);
                            if (subExprValue->isConstant()) {
                                return CPValue::makeConstant(llvm::APSInt(
                                    subExprValue->getConstantValue(),
                                    expr->getType()->isSignedIntegerType()));
                            } else  {
                                return subExprValue;
                            }
                        }
                        default:
                            return CPValue::getNAC();
                    }
                } else if (auto* declRef = llvm::dyn_cast<clang::DeclRefExpr>(expr)) {
                    if (auto* varDecl = llvm::dyn_cast<clang::VarDecl>(declRef->getDecl()))
                        if (checkClangVarDeclType(varDecl))
                            return inFact->get(mapVars.at(varDecl));
                    return CPValue::getNAC();
                } else if (auto* unaryOp = llvm::dyn_cast<clang::UnaryOperator>(expr)) {
                    auto subExpr = unaryOp->getSubExpr();
                    switch (unaryOp->getOpcode()) {
                        case clang::UnaryOperatorKind::UO_Plus:
                            return calculateExprCPValue(subExpr, inFact);
                        case clang::UnaryOperatorKind::UO_Minus: {
                            auto subExprValue = calculateExprCPValue(subExpr, inFact);
                            if (subExprValue->isNAC()) {
                                return CPValue::getNAC();
                            } else if (subExprValue->isConstant()) {
                                return CPValue::makeConstant(-subExprValue->getConstantValue());
                            } else {
                                return CPValue::getUndef();
                            }
                        }
                        default:
                            return CPValue::getNAC();
                    }
                } else if (auto* binaryOperator = llvm::dyn_cast<clang::BinaryOperator>(expr)) {
                    auto lhs = binaryOperator->getLHS();
                    auto rhs = binaryOperator->getRHS();
                    auto rhsValue = calculateExprCPValue(rhs, inFact);
                    if (binaryOperator->getOpcode() == clang::BinaryOperatorKind::BO_Assign) {
                        return rhsValue;
                    }
                    auto lhsValue = calculateExprCPValue(lhs, inFact);
                    if (lhsValue->isNAC() || rhsValue->isNAC()) {
                        if ((binaryOperator->getOpcode() == clang::BinaryOperatorKind::BO_Div 
                                || binaryOperator->getOpcode() == clang::BinaryOperatorKind::BO_DivAssign
                                || binaryOperator->getOpcode() == clang::BinaryOperatorKind::BO_Rem
                                || binaryOperator->getOpcode() == clang::BinaryOperatorKind::BO_RemAssign)
                            && rhsValue->isConstant() && rhsValue->getConstantValue().isZero()) {
                            return CPValue::getUndef();
                        } else {
                            return CPValue::getNAC();
                        }
                    } else if (lhsValue->isConstant() && rhsValue->isConstant()) {
                        auto lhsConstant = lhsValue->getConstantValue();
                        auto rhsConstant = rhsValue->getConstantValue();
                        switch (binaryOperator->getOpcode()) {
                            case clang::BinaryOperatorKind::BO_Add:
                            case clang::BinaryOperatorKind::BO_AddAssign:
                                return CPValue::makeConstant(lhsConstant + rhsConstant);
                            case clang::BinaryOperatorKind::BO_Sub:
                            case clang::BinaryOperatorKind::BO_SubAssign:
                                return CPValue::makeConstant(lhsConstant - rhsConstant);
                            case clang::BinaryOperatorKind::BO_Mul:
                            case clang::BinaryOperatorKind::BO_MulAssign:
                                return CPValue::makeConstant(lhsConstant * rhsConstant);
                            case clang::BinaryOperatorKind::BO_Div:
                            case clang::BinaryOperatorKind::BO_DivAssign:
                                if (rhsConstant.isZero()) {
                                    return CPValue::getUndef();
                                } else {
                                    return CPValue::makeConstant(lhsConstant / rhsConstant);
                                }
                            case clang::BinaryOperatorKind::BO_Rem:
                            case clang::BinaryOperatorKind::BO_RemAssign:
                                if (rhsConstant.isZero()) {
                                    return CPValue::getUndef();
                                } else {
                                    return CPValue::makeConstant(lhsConstant % rhsConstant);
                                }
                            case clang::BinaryOperatorKind::BO_And:
                            case clang::BinaryOperatorKind::BO_AndAssign:
                                return CPValue::makeConstant(lhsConstant & rhsConstant);
                            case clang::BinaryOperatorKind::BO_Or:
                            case clang::BinaryOperatorKind::BO_OrAssign:
                                return CPValue::makeConstant(lhsConstant | rhsConstant);
                            case clang::BinaryOperatorKind::BO_Xor:
                            case clang::BinaryOperatorKind::BO_XorAssign:
                                return CPValue::makeConstant(lhsConstant ^ rhsConstant);
                            case clang::BinaryOperatorKind::BO_Shl:
                            case clang::BinaryOperatorKind::BO_ShlAssign:
                                return CPValue::makeConstant(lhsConstant << rhsConstant.getLimitedValue());
                            case clang::BinaryOperatorKind::BO_Shr:
                            case clang::BinaryOperatorKind::BO_ShrAssign:
                                return CPValue::makeConstant(lhsConstant >> rhsConstant.getLimitedValue());
                            default:
                                return CPValue::getNAC();
                        }
                    } else {
                        return CPValue::getUndef();
                    }
                } else {
                    return CPValue::getNAC();
                }
            }
        };

        return std::make_unique<Analysis>(cfg);
    }

}
