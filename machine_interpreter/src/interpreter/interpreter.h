#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "command.h"
#include "bin_op_executor.h"
#include "input_provider.h"
#include "output_provider.h"
#include "value.h"

class Interpreter {
public:
    Interpreter(std::unique_ptr<InputProvider> input_provider,
                std::unique_ptr<OutputProvider> output_provider);

    void interpret(std::vector<Command> program);

private:
    std::unique_ptr<InputProvider> input_provider_;
    std::unique_ptr<OutputProvider> output_provider_;
    BinOpExecutor bin_op_executor_;

    void print_impl(const Value& to_print, bool top_level = true);
    std::pair<Value, std::string_view> parse_impl(std::string_view input);
    std::pair<Value, std::string_view> parse_array(std::string_view input);
    std::pair<Value, std::string_view> parse_sexp_simple(std::string_view input);
    std::pair<Value, std::string_view> parse_sexp(std::string_view input);
    std::pair<Value, std::string_view> parse_num(std::string_view input);
    std::pair<Value, std::string_view> parse_string(std::string_view input);
    std::pair<Value, std::string_view> parse_char(std::string_view input);
    std::pair<List, std::string_view> parse_values(std::string_view input, char stop);
    std::string_view trim(std::string_view input);
};
