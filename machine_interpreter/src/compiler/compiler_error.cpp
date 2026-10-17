#include "compiler_error.h"

#include <stdexcept>

[[noreturn]] void fail(const Token &token, const std::string &message) {
    throw std::invalid_argument("Line " + std::to_string(token.line) + ", column " + std::to_string(token.column) + ": " + message);
}
