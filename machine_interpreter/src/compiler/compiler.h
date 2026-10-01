#pragma once

#include <string_view>
#include <vector>

#include "command.h"

std::vector<Command> compile(std::string_view source);
