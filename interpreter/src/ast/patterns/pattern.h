#pragma once

#include <memory>

#include "interpreter.h"
#include "node.h"

class Pattern : public Node {
public:
    virtual bool check(Interpreter& interpreter, const Value& value) const = 0;
};

using PatternPtr = std::unique_ptr<Pattern>;