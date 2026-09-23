#pragma once

#include <memory>

#include "command.h"
#include "bin_op_executor.h"
#include "input_provider.h"
#include "output_provider.h"

class Interpreter {
public:
    Interpreter(std::unique_ptr<InputProvider> input_provider,
                std::unique_ptr<OutputProvider> output_provider);

    void interpret(std::vector<Command> program);
private:
    std::unique_ptr<InputProvider> input_provider_;
    std::unique_ptr<OutputProvider> output_provider_;
    BinOpExecutor bin_op_executor_;
};