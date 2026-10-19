#pragma once

#include <string>

#include "token.h"

[[noreturn]] void fail(const Token &token, const std::string &message);
