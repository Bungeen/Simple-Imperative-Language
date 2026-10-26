#include <unordered_map>
#include <string>
#include <string_view>
#include <cstdint>
#include <variant>
#include <stack>
#include <vector>
#include <memory>
#include <stdexcept>
#include <limits>
#include <charconv>

#include "command.h"
#include "interpreter.h"
#include "value.h"

Interpreter::Interpreter(std::unique_ptr<InputProvider> input_provider,
                         std::unique_ptr<OutputProvider> output_provider) 
    : input_provider_(std::move(input_provider))
    , output_provider_(std::move(output_provider))
    , bin_op_executor_()
{}

std::string_view Interpreter::trim(std::string_view input) {
    if (input.empty())
        return input;

    std::size_t left_bound = input.find_first_not_of(" \t\n\r\v\f");
    if (left_bound == std::string_view::npos)
        return {};
    std::size_t right_bound = input.find_last_not_of(" \t\n\r\v\f");

    return input.substr(left_bound, right_bound - left_bound + 1);
}

std::pair<Value, std::string_view> Interpreter::parse_num(std::string_view input) {
    std::int64_t value = 0;
    auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
    if (ec != std::errc())
        throw std::invalid_argument("invalid input");

    std::string_view rest = trim(std::string_view(ptr, input.data() + input.size() - ptr));
    return {value, rest};
}

std::pair<Value, std::string_view> Interpreter::parse_string(std::string_view input) {
    std::string_view rest = input.substr(1);
    std::size_t str_end = rest.find('\"');
    if (str_end == std::string_view::npos)
        throw std::invalid_argument("invalid input");

    Value result = std::make_shared<std::string>(rest.substr(0, str_end));
    rest = trim(rest.substr(str_end + 1));
    return {result, rest};
}

std::pair<Value, std::string_view> Interpreter::parse_char(std::string_view input) {
    if (input.size() < 3 || input[2] != '\'')
        throw std::invalid_argument("invalid input");

    std::int64_t result = static_cast<unsigned char>(input[1]);
    std::string_view rest = trim(input.substr(3));
    return {result, rest};
}

std::pair<List, std::string_view> Interpreter::parse_values(std::string_view input, char stop) {
    List result;
    std::string_view rest = trim(input.substr(1));

    while (!rest.empty() && rest.front() != stop) {
        auto [value, next_rest] = parse_impl(rest);
        result.push_back(value);
        rest = trim(next_rest);

        if (rest.empty() || (rest.front() != stop && rest.front() != ','))
            throw std::invalid_argument("invalid input");

        if (rest.front() == ',')
            rest = trim(rest.substr(1));
    }

    if (rest.empty() || rest.front() != stop)
        throw std::invalid_argument("invalid input");

    rest = trim(rest.substr(1));
    return {std::move(result), rest};
}

std::pair<Value, std::string_view> Interpreter::parse_array(std::string_view input) {
    auto [result, rest] = parse_values(input, ']');
    return {std::make_shared<List>(std::move(result)), rest};
}

std::pair<Value, std::string_view> Interpreter::parse_sexp_simple(std::string_view input) {
    auto [result, rest] = parse_values(input, '}');
    auto sexp = std::make_shared<Sexp>();
    sexp->tag = "cons";
    sexp->values = std::move(result);
    return {sexp, rest};
}

std::pair<Value, std::string_view> Interpreter::parse_sexp(std::string_view input) {
    std::string tag;
    while (!input.empty() && ((input.front() >= 'A' && input.front() <= 'Z') || 
                              (input.front() >= 'a' && input.front() <= 'z'))) {
        tag += input.front();
        input = input.substr(1);
    }

    input = trim(input);
    if (input.empty() || input.front() != '(')
        throw std::invalid_argument("invalid input");

    auto [values, rest] = parse_values(input, ')');
    auto sexp = std::make_shared<Sexp>();
    sexp->tag = std::move(tag);
    sexp->values = std::move(values);
    return {sexp, rest};
}

std::pair<Value, std::string_view> Interpreter::parse_impl(std::string_view input) {
    if (input.empty())
        throw std::invalid_argument("invalid input");

    input = trim(input);
    if (input.front() == '[')
        return parse_array(input);
    if ((input.front() >= 'A' && input.front() <= 'Z') || (input.front() >= 'a' && input.front() <= 'z'))
        return parse_sexp(input);
    if (input.front() == '{')
        return parse_sexp_simple(input);
    if (input.front() == '\"')
        return parse_string(input);
    if (input.front() == '\'')
        return parse_char(input);

    return parse_num(input);
}

void Interpreter::print_impl(const Value& to_print, bool top_level) {
    auto op = Overload{
        [](auto) {
            throw std::logic_error("Invalid type for print");
        },
        [this, top_level](std::int64_t num) {
            output_provider_->write(std::to_string(num));
            if (top_level) output_provider_->write(" ");
        },
        [this, top_level](const std::shared_ptr<std::string>& str) {
            output_provider_->write(*str);
            if (top_level) output_provider_->write(" ");
        },
        [this, top_level](const std::shared_ptr<List>& list) {
            output_provider_->write("[");
            for (std::size_t i = 0; i < list->size(); ++i) {
                if (i > 0) output_provider_->write(", ");
                print_impl((*list)[i], false);
            }
            output_provider_->write("]");
            if (top_level) output_provider_->write(" ");
        },
        [this, top_level](const std::shared_ptr<Sexp>& sexp) {
            output_provider_->write(sexp->tag);
            output_provider_->write(": [");
            for (std::size_t i = 0; i < sexp->values.size(); ++i) {
                if (i > 0) output_provider_->write(", ");
                print_impl(sexp->values[i], false);
            }
            output_provider_->write("]");
            if (top_level) output_provider_->write(" ");
        }
    };
    std::visit(op, to_print);
}

void Interpreter::interpret(std::vector<Command> program) {
    std::unordered_map<std::string, std::size_t> labels;

    for (std::size_t i = 0; i < program.size(); i++) {
        if (program[i].operation == Operation::Label)
            labels[std::get<std::string>(program[i].argument)] = i;
    }

    std::stack<Value, std::vector<Value>> stack;
    std::unordered_map<std::string, Value> variables;

    std::size_t pointer = 0;

    while (pointer < program.size()) {
        switch (program[pointer].operation) {
            case Operation::Read: {
                std::string input = input_provider_->get_input();
                bool parsed = false;
                try {
                    auto [val, rest] = parse_impl(input);
                    if (rest.empty()) {
                        stack.push(val);
                        parsed = true;
                    }
                } catch (...) {}

                if (!parsed) {
                    try {
                        std::size_t pos = 0;
                        std::int64_t val = std::stoll(input, &pos);
                        if (pos == input.size()) {
                            stack.push(val);
                        } else {
                            stack.push(std::make_shared<std::string>(std::move(input)));
                        }
                    } catch (...) {
                        stack.push(std::make_shared<std::string>(std::move(input)));
                    }
                }
                break;
            }
            case Operation::Write: {
                print_impl(stack.top());
                stack.pop();
                break;
            }
            case Operation::Ld:
                stack.push(variables.at(std::get<std::string>(program[pointer].argument)));
                break;
            case Operation::St:
                variables[std::get<std::string>(program[pointer].argument)] = stack.top();
                stack.pop();
                break;
            case Operation::Const:
                stack.push(std::get<std::int64_t>(program[pointer].argument));
                break;
            case Operation::String:
                stack.push(std::make_shared<std::string>(std::get<std::string>(program[pointer].argument)));
                break;
            case Operation::Array: {
                std::int64_t count = std::get<std::int64_t>(program[pointer].argument);
                auto list = std::make_shared<List>(static_cast<std::size_t>(count));
                for (std::int64_t i = count - 1; i >= 0; --i) {
                    (*list)[static_cast<std::size_t>(i)] = std::move(stack.top());
                    stack.pop();
                }
                stack.push(list);
                break;
            }
            case Operation::Sexp: {
                const auto& arg = std::get<SexpArg>(program[pointer].argument);
                auto sexp = std::make_shared<Sexp>();
                sexp->tag = arg.tag;
                sexp->values.resize(static_cast<std::size_t>(arg.count));
                for (std::int64_t i = arg.count - 1; i >= 0; --i) {
                    sexp->values[static_cast<std::size_t>(i)] = std::move(stack.top());
                    stack.pop();
                }
                stack.push(sexp);
                break;
            }
            case Operation::Length: {
                Value val = stack.top();
                stack.pop();
                auto length_op = Overload{
                    [](auto arg) -> std::int64_t {
                        throw std::logic_error("Invalid type for length");
                    },
                    [](const std::shared_ptr<List>& list) -> std::int64_t {
                        return static_cast<std::int64_t>(list->size());
                    },
                    [](const std::shared_ptr<Sexp>& sexp) -> std::int64_t {
                        return static_cast<std::int64_t>(sexp->values.size());
                    },
                    [](const std::shared_ptr<std::string>& str) -> std::int64_t {
                        return static_cast<std::int64_t>(str->size());
                    }
                };
                stack.push(std::visit(length_op, val));
                break;
            }
            case Operation::Elem: {
                Value idx_val = stack.top();
                stack.pop();
                Value list_val = stack.top();
                stack.pop();

                auto elem_op = Overload{
                    [](auto arg1, auto arg2) -> Value {
                        throw std::logic_error("Invalid types for element access");
                    },
                    [](const std::shared_ptr<List>& list, std::int64_t pos) -> Value {
                        if (pos < 0)
                            pos = static_cast<std::int64_t>(list->size()) + pos;
                        if (pos < 0 || static_cast<std::size_t>(pos) >= list->size())
                            throw std::out_of_range("list index out of range");
                        return (*list)[static_cast<std::size_t>(pos)];
                    },
                    [](const std::shared_ptr<Sexp>& sexp, std::int64_t pos) -> Value {
                        if (pos < 0)
                            pos = static_cast<std::int64_t>(sexp->values.size()) + pos;
                        if (pos < 0 || static_cast<std::size_t>(pos) >= sexp->values.size())
                            throw std::out_of_range("sexp index out of range");
                        return sexp->values[static_cast<std::size_t>(pos)];
                    },
                    [](const std::shared_ptr<std::string>& str, std::int64_t pos) -> Value {
                        if (pos < 0)
                            pos = static_cast<std::int64_t>(str->size()) + pos;
                        if (pos < 0 || static_cast<std::size_t>(pos) >= str->size())
                            throw std::out_of_range("string index out of range");
                        return static_cast<std::int64_t>(static_cast<unsigned char>((*str)[static_cast<std::size_t>(pos)]));
                    }
                };
                stack.push(std::visit(elem_op, list_val, idx_val));
                break;
            }
            case Operation::Set: {
                Value val = stack.top();
                stack.pop();
                Value idx_val = stack.top();
                stack.pop();
                Value list_val = stack.top();
                stack.pop();

                auto set_op = Overload{
                    [](auto arg1, auto arg2, auto arg3) {
                        throw std::logic_error("Invalid types for element assignment");
                    },
                    [](const std::shared_ptr<List>& list, std::int64_t pos, const auto& v) {
                        if (pos < 0)
                            pos = static_cast<std::int64_t>(list->size()) + pos;
                        if (pos < 0 || static_cast<std::size_t>(pos) >= list->size())
                            throw std::out_of_range("list index out of range");
                        (*list)[static_cast<std::size_t>(pos)] = Value(v);
                    },
                    [](const std::shared_ptr<Sexp>& sexp, std::int64_t pos, const auto& v) {
                        if (pos < 0)
                            pos = static_cast<std::int64_t>(sexp->values.size()) + pos;
                        if (pos < 0 || static_cast<std::size_t>(pos) >= sexp->values.size())
                            throw std::out_of_range("sexp index out of range");
                        sexp->values[static_cast<std::size_t>(pos)] = Value(v);
                    },
                    [](const std::shared_ptr<std::string>& str, std::int64_t pos, std::int64_t char_code) {
                        if (pos < 0)
                            pos = static_cast<std::int64_t>(str->size()) + pos;
                        if (pos < 0 || static_cast<std::size_t>(pos) >= str->size())
                            throw std::out_of_range("string index out of range");
                        if (char_code > std::numeric_limits<char>::max() || char_code < std::numeric_limits<char>::min())
                            throw std::out_of_range("can not assign to char integer not from char limits");
                        (*str)[static_cast<std::size_t>(pos)] = static_cast<char>(char_code);
                    },
                    [](const std::shared_ptr<std::string>& str, std::int64_t pos, const std::shared_ptr<std::string>& s) {
                        if (pos < 0)
                            pos = static_cast<std::int64_t>(str->size()) + pos;
                        if (pos < 0 || static_cast<std::size_t>(pos) >= str->size())
                            throw std::out_of_range("string index out of range");
                        if (s->size() != 1)
                            throw std::logic_error("expected 1-char string");
                        (*str)[static_cast<std::size_t>(pos)] = (*s)[0];
                    }
                };
                std::visit(set_op, list_val, idx_val, val);
                break;
            }
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
                if (!stack.top().to_bool())
                    pointer = labels[std::get<std::string>(program[pointer].argument)];
                stack.pop();
                break;
            case Operation::Jnz:
                if (stack.top().to_bool())
                    pointer = labels[std::get<std::string>(program[pointer].argument)];
                stack.pop();
                break;
        }

        pointer++;
    }
}
