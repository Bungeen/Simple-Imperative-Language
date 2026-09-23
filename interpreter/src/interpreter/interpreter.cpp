#include <memory>
#include <stdexcept>

#include "interpreter.h"
#include "ast.h"

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

Value Interpreter::Visit(const Const& node) {
    return node.get_value();
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
    Value& dst = var_table_[node.get_dst()];
    const Value& src = node.get_src().evaluate(*this);

    dst = src;
}

void Interpreter::Visit(const ReadStmt& node) {
    try {
        var_table_[node.get_variable_name()] = std::stoll(input_provider_->get_input());
    } catch (std::invalid_argument exception) {
        throw std::invalid_argument("Input is not float constant");
    } catch (std::out_of_range exception) {
        throw std::invalid_argument("Float constant from input is too big");
    }
}

void Interpreter::Visit(const WriteStmt& node) {
    output_provider_->write(std::to_string(node.get_value().evaluate(*this)) + " ");
}

void Interpreter::Visit(const WhileStmt& node) {
    while (node.get_condition().evaluate(*this)) {
        node.get_body().execute(*this);
    }
}

void Interpreter::Visit(const DoWhileStmt& node) {
    do {
        node.get_body().execute(*this);
    } while (node.get_condition().evaluate(*this));
}

void Interpreter::Visit(const IfStmt& node) {
    if (node.get_condition().evaluate(*this)) {
        node.get_then_branch().execute(*this);
    }
    else {
        node.get_else_branch().execute(*this);
    }
}