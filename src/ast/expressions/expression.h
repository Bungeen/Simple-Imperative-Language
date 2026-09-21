#pragma once

#include <memory>


#include "node.h"

class Expr : public Node {};

using ExprPtr = std::unique_ptr<Expr>;
