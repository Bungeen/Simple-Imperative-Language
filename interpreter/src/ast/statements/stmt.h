#pragma once

#include <memory>

#include "node.h"
#include "interpreter.h"

class Stmt : public Node {
public:
    virtual void execute(Interpreter& interpreter) const = 0;
};

using StmtPtr = std::unique_ptr<Stmt>;
