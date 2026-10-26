#include <memory>
#include <stdexcept>
#include <limits>
#include <string_view>
#include <ranges>
#include <charconv>

#include "interpreter.h"
#include "ast.h"
#include "overload.h"

Interpreter::Interpreter(std::unique_ptr<InputProvider> input_provider,
                         std::unique_ptr<OutputProvider> output_provider)
    : var_table_()
    , input_provider_(std::move(input_provider))
    , output_provider_(std::move(output_provider))
    , bin_op_executor_()
{}

Value Interpreter::Visit(const Var& node) {
    if (!var_table_.contains(node.get_var_name()))
        throw std::logic_error("Not declared variable was used\n");

    return var_table_[node.get_var_name()];
}

Value Interpreter::Visit(const ElemAccess& node) {
    const Value& list = node.get_list().evaluate(*this);
    const Value& selector = node.get_selector().evaluate(*this);

    auto value_access = Overload{
        [](auto arg1, auto arg2)->Value {
            throw std::logic_error("Invalid types for element access");
        },
        [](const std::shared_ptr<List>& list, std::int64_t pos)->Value {
            if (pos < 0)
                pos = list->size() + pos;
            if (pos < 0 || static_cast<size_t>(pos) >= list->size())
                throw std::out_of_range("list index out of range");
            return list->at(static_cast<size_t>(pos));
        },
        [](const std::shared_ptr<Sexp>& sexp, std::int64_t pos)->Value {
            if (pos < 0)
                pos = sexp->values.size() + pos;
            if (pos < 0 || static_cast<size_t>(pos) >= sexp->values.size())
                throw std::out_of_range("list index out of range");
            return sexp->values.at(static_cast<size_t>(pos));
        },
        [](const std::shared_ptr<std::string>& str, std::int64_t pos)->Value {
            if (pos < 0)
                pos = str->size() + pos;
            if (pos < 0 || static_cast<size_t>(pos) >= str->size())
                throw std::out_of_range("string index out of range");
            return static_cast<std::int64_t>(1, str->at(static_cast<size_t>(pos)));
        }
    };

    return std::visit(value_access, list, selector);
}

Value Interpreter::Visit(const Length& node) {
    const Value& list = node.get_list().evaluate(*this);

    auto length_access = Overload{
        [](auto arg1)->Value {
            throw std::logic_error("Invalid types for length");
        },
        [](const std::shared_ptr<List>& list)->Value {
            return static_cast<std::int64_t>(list->size());
        },
        [](const std::shared_ptr<Sexp>& sexp)->Value {
            return static_cast<std::int64_t>(sexp->values.size());
        }
    };

    return std::visit(length_access, list);
}

Value Interpreter::Visit(const Const& node) {
    return node.get_value();
}

Value Interpreter::Visit(const ListExpr& node) {
    const std::vector<ExprPtr>& params = node.get_params();

    std::shared_ptr<List> result = std::make_shared<List>();

    for (const auto& param : params) {
        result->push_back(param->evaluate(*this));
    }

    return result;
}

Value Interpreter::Visit(const SexpExpr& node) {
    const std::vector<ExprPtr>& params = node.get_params();
    const std::string& tag = node.get_tag();

    List result{};

    for (const auto& param : params) {
        result.push_back(param->evaluate(*this));
    }

    return std::make_shared<Sexp>(tag, result);
}

Value Interpreter::Visit(const BinOpExpr& node) {
    const Value& lhs = node.get_lhs().evaluate(*this);
    const Value& rhs = node.get_rhs().evaluate(*this);

    return bin_op_executor_.ExecuteBinOp(node.get_operation(), lhs, rhs);
}

void Interpreter::Visit(const SkipStmt& node) {
    return;
}

void Interpreter::Visit(const SeqStmt& node) {
    node.get_lhs().execute(*this);
    node.get_rhs().execute(*this);
}

void Interpreter::Visit(const AssignStmt& node) {
    
    const Value& src = node.get_src().evaluate(*this);

    var_table_[node.get_dst_name()] = src;
}

void Interpreter::Visit(const AssignElemStmt& node) {
    
    const Value& src = node.get_src().evaluate(*this);
    const Value& dst = node.get_dst().evaluate(*this);
    const Value& idx = node.get_idx().evaluate(*this);
    
    auto assign = Overload{
        [](auto arg1, auto arg2, auto arg3) {
            throw std::logic_error("Invalid types for element assignment");
        },
        [](const std::shared_ptr<List>& list, std::int64_t pos, auto value) {
            if (pos < 0)
                pos = list->size() + pos;
            if (pos < 0 || static_cast<size_t>(pos) >= list->size())
                throw std::out_of_range("list index out of range");

            list->at(pos) = value;
        },
        [](const std::shared_ptr<std::string>& str, std::int64_t pos, std::int64_t value) {
            if (pos < 0)
                pos = str->size() + pos;
            if (pos < 0 || static_cast<size_t>(pos) >= str->size())
                throw std::out_of_range("string index out of range");
            if ((value > std::numeric_limits<char>::max()) || (value < std::numeric_limits<char>::min()))
                throw std::out_of_range("can not assign to char integer not from char limits");

            str->at(pos) = static_cast<char>(value);
        },
        [](const std::shared_ptr<Sexp>& sexp, std::int64_t pos, auto value) {
            if (pos < 0)
                pos = sexp->values.size() + pos;
            if (pos < 0 || static_cast<size_t>(pos) >= sexp->values.size())
                throw std::out_of_range("string index out of range");

            sexp->values.at(pos) = value;
        }
    };

    std::visit(assign, dst, idx, src);
}

std::string_view Interpreter::trim(std::string_view input) {
    if (input.empty())
        return input;

    std::size_t left_bound = input.find_first_not_of(" \t\n\r\v\f");
    std::size_t right_bound = input.find_last_not_of(" \t\n\r\v\f");

    return input.substr(left_bound, right_bound - left_bound + 1);
}

std::pair<Value, std::string_view> Interpreter::parse_num(std::string_view input) {
    std::int64_t value;

     auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);

     if (ec != std::errc())
        throw std::invalid_argument("invalid input");

    std::string_view rest = trim(std::string_view(ptr, input.data() + input.size()));

    return std::pair(value, rest);
}

std::pair<Value, std::string_view> Interpreter::parse_string(std::string_view input) {
    std::string_view rest = input.substr(1, input.size() - 1);

    std::size_t str_end = rest.find("\"");

    if (str_end == rest.size())
        throw std::invalid_argument("invalid input");

    Value result = std::make_shared<std::string>(rest.substr(0, str_end));

    rest = rest.substr(str_end + 1, rest.size() - str_end - 1);

    return std::pair(result, rest);
}

std::pair<Value, std::string_view> Interpreter::parse_char(std::string_view input) {
    if ((input.size() < 3) || input[2] != '\'')
        throw std::invalid_argument("invalid input");

    std::int64_t result = input[1];
    std::string_view rest = input.substr(3, input.size() - 3);

    return std::pair(result, rest);
}

std::pair<List, std::string_view>  Interpreter::parse_values(std::string_view input, char stop){
    List result;

    std::string_view rest = trim(input.substr(1, input.size() - 1));

    while (rest[0] != stop) {
        auto [value, next_rest] = parse_impl(rest);

        result.push_back(value);

        rest = trim(next_rest);

        if (rest.empty() || ((rest[0] != stop) && (rest[0] != ',')))
            throw std::invalid_argument("invalid input");

        if (rest[0] == ',')
            rest = trim(rest.substr(1, rest.size() - 1));
    }

    rest = rest.substr(1, rest.size() - 1);

    return std::pair(std::move(result), rest);
}

std::pair<Value, std::string_view> Interpreter::parse_array(std::string_view input) {
    auto [result, rest] = parse_values(input, ']');

    return std::pair(std::make_shared<List>(std::move(result)), rest);
}

std::pair<Value, std::string_view> Interpreter::parse_sexp_simple(std::string_view input) {
    auto [result, rest] = parse_values(input, '}');

    return std::pair(std::make_shared<Sexp>("cons", std::move(result)), rest);
}

std::pair<Value, std::string_view> Interpreter::parse_sexp(std::string_view input) {
    std::string tag;

    while (((input.front() >= 'A') && (input.front() <= 'Z')) || 
           ((input.front() >= 'a') && (input.front() <= 'z'))) {
        tag += input.front();
        input = input.substr(1, input.size() - 1);
    }

    input = trim(input);

    if (input.front() != '(')
        throw std::invalid_argument("invalid input");

    auto [values, rest] = parse_values(input, ')');

    return std::pair(std::make_shared<Sexp>(tag, std::move(values)), rest);
}

std::pair<Value, std::string_view> Interpreter::parse_impl(std::string_view input) {
    if (input.empty())
        throw std::invalid_argument("invalid input");

    input = trim(input);

    if (input.front() == '[')
        return parse_array(input);
    else if ((input.front() >= 'A') && (input.front() <= 'Z'))
        return parse_sexp(input);
    else if (input.front() == '{')
        return parse_sexp_simple(input);
    else if (input.front() == '\"') 
        return parse_string(input);
    else if (input.front() == '\'')
        return parse_char(input);
    else
        return parse_num(input);
}

void Interpreter::Visit(const ReadStmt& node) {
    auto [value, rest] = parse_impl(input_provider_->get_input());

    if (!rest.empty()) {
        throw std::invalid_argument("invalid input");
    }

    var_table_[node.get_variable_name()] = value;
}

void Interpreter::print_impl(const Value& to_print) {
    auto operations = Overload{
        [](auto arg1) {
            throw std::logic_error("Invalid types for print");
        },
        [this](std::int64_t num) {
            output_provider_->write(std::to_string(num));
            output_provider_->write(" ");
        },
        [this](const std::shared_ptr<List>& list) {
            output_provider_->write("[");
            bool is_first = true;
            for(const auto& elem : *list) {
                if (!is_first)
                    output_provider_->write(", ");
                else
                    is_first = false;
                print_impl(elem);
            }
            output_provider_->write("] ");
        },
        [this](const std::shared_ptr<Sexp>& sexp) {
            output_provider_->write(sexp->tag);
            output_provider_->write(": [");
            bool is_first = true;
            for(const auto& elem : sexp->values) {
                if (!is_first)
                    output_provider_->write(", ");
                else
                    is_first = false;
                print_impl(elem);
            }
            output_provider_->write("] ");
        },
        [this](const std::shared_ptr<std::string>& str) {
            output_provider_->write(*str);
            output_provider_->write(" ");
        },
    };

    std::visit(operations, to_print);
}

void Interpreter::Visit(const WriteStmt& node) {
    const Value& to_print = node.get_value().evaluate(*this);

    print_impl(to_print);
}

void Interpreter::Visit(const WhileStmt& node) {
    while (node.get_condition().evaluate(*this).to_bool()) {
        node.get_body().execute(*this);
    }
}

void Interpreter::Visit(const DoWhileStmt& node) {
    do {
        node.get_body().execute(*this);
    } while (node.get_condition().evaluate(*this).to_bool());
}

void Interpreter::Visit(const IfStmt& node) {
    if (node.get_condition().evaluate(*this).to_bool()) {
        node.get_then_branch().execute(*this);
    }
    else {
        node.get_else_branch().execute(*this);
    }
}