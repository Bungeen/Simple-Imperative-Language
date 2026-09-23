#include <unordered_map>
#include <string>
#include <cstdint>
#include <variant>
#include <stack>

#include "command.h"
#include "interpreter.h"

Interpreter::Interpreter(std::unique_ptr<InputProvider> input_provider,
                         std::unique_ptr<OutputProvider> output_provider) 
    : input_provider_(std::move(input_provider))
    , output_provider_(std::move(output_provider))
    , bin_op_executor_()
{}

void Interpreter::interpret(std::vector<Command> program) {
    std::unordered_map<std::string, std::size_t> labels;

    for (std::size_t i = 0; i < program.size(); i++) {
        if (program[i].operation == Operation::Label)
            labels[std::get<std::string>(program[i].argument)] = i;
    }

    std::stack<Value> stack;
    std::unordered_map<std::string, Value> variables;

    std::size_t pointer = 0;

    while (pointer < program.size()) {
        switch (program[pointer].operation) {
            case Operation::Read: 
                stack.push(std::stoll(input_provider_->get_input())); 
                break;
            case Operation::Write:
                output_provider_->write(std::to_string(stack.top()) + " ");
                stack.pop();
                break;
            case Operation::Ld:
                stack.push(variables.at(std::get<std::string>(program[pointer].argument)));
                break;
            case Operation::St:
                variables[std::get<std::string>(program[pointer].argument)] = stack.top();
                break;
            case Operation::Const:
                stack.push(std::get<Value>(program[pointer].argument));
                break;
            case Operation::Binop: {
                Value rhs = stack.top();
                stack.pop();
                Value lhs = stack.top();
                stack.pop();
                BinOp operation = std::get<BinOp>(program[pointer].argument);
                stack.push(bin_op_executor_.ExecuteBinOp(operation, lhs, rhs));
                break;
            }
            case Operation::Label:
                break;
            case Operation::Jmp:
                pointer = labels[std::get<std::string>(program[pointer].argument)];
                break;
            case Operation::Jz:
                if (!stack.top())
                    pointer = labels[std::get<std::string>(program[pointer].argument)];
                stack.pop();
                break;
            case Operation::Jnz:
                if (stack.top())
                    pointer = labels[std::get<std::string>(program[pointer].argument)];
                stack.pop();
                break;
        }

        pointer++;
    }
}