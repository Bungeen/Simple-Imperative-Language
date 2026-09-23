#pragma once

#include <vector>
#include <variant>
#include <cstdint>
#include <string>

#include "bin_op.h"

enum class Operation {
    Read,
    Write,
    Ld,
    St,
    Const,
    Binop,
    Label,
    Jmp,
    Jz,
    Jnz
};

using Value = std::int64_t;

using CommandArgument = std::variant<std::string, Value, BinOp>;

struct Command {
    Operation operation;
    CommandArgument argument;
};