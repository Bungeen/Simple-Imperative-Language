#include <memory>
#include <stdexcept>
#include <limits>

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
        }
    };

    std::visit(assign, dst, idx, src);
}

void Interpreter::Visit(const ReadStmt& node) {
    try {
        var_table_[node.get_variable_name()] = std::make_shared<std::string>(input_provider_->get_input());
    } catch (std::invalid_argument exception) {
        throw std::invalid_argument("Input is not float constant");
    } catch (std::out_of_range exception) {
        throw std::invalid_argument("Float constant from input is too big");
    }
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