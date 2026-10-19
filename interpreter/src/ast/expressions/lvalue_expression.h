#pragma once

#include "value.h"
#include "expression.h"

class LvalueExpr : public Expr {
public:
    virtual Value& evaluate_ref(Interpreter& interpreter) const = 0;
};

using LvalueExprPtr = std::unique_ptr<LvalueExpr>;