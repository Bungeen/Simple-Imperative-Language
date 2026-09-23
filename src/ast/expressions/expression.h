#pragma once

#include <memory>

#include "interpreter.h"
#include "node.h"

class Expr : public Node {
public:
    virtual Value evaluate(Interpreter& interpreter) const = 0;
};

using ExprPtr = std::unique_ptr<Expr>;
