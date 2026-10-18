#pragma once

#include <vector>
#include <nlohmann/json.hpp>

#include "command.h"

std::vector<Command> compile(const nlohmann::json& input);
