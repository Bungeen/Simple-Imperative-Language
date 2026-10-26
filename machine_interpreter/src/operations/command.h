#pragma once

#include <vector>
#include <variant>
#include <cstdint>
#include <string>

#include "bin_op.h"
#include "value.h"

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
    Jnz,
    Array,
    String,
    Length,
    Elem,
    Set,
    Sexp
};

struct SexpArg {
    std::string tag;
    std::int64_t count = 0;
};

using CommandArgument = std::variant<std::monostate, std::string, std::int64_t, BinOp, SexpArg>;

struct Command {
    Operation operation;
    CommandArgument argument{};
};
