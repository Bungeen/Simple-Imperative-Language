#pragma once

#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "bin_op.h"
#include "command.h"

const std::unordered_map<std::string, BinOp> kBinOpMap{
    {"!!", BinOp::Or},
    {"&&", BinOp::And},
    {"==", BinOp::Equal},
    {"!=", BinOp::NotEqual},
    {"<=", BinOp::LessEqual},
    {">=", BinOp::GreaterEqual},
    {">", BinOp::Greater},
    {"<", BinOp::Less},
    {"+", BinOp::Add},
    {"-", BinOp::Subtract},
    {"*", BinOp::Multiply},
    {"/", BinOp::Divide},
    {"%", BinOp::Modulo}
};

std::vector<Command> parse(const nlohmann::json& input);