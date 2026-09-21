#pragma once

#include <memory>

#include "node.h"

class Stmt : public Node {};

using StmtPtr = std::unique_ptr<Stmt>;
