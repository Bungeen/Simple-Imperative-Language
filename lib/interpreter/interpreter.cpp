#include <memory>
#include <stdexcept>

#include "interpreter.h"

Interpreter::Interpreter(std::unique_ptr<IInputProvider> input_provider,
                         std::unique_ptr<IOutputProvider> output_provider)
    : var_table_()
    , input_provider_(std::move(input_provider))
    , output_provider_(std::move(output_provider))
    , bin_op_executor_()
{}

Value& Interpreter::VisitLvalue(const VarNode& node) {
    return var_table_[node.GetVarName()];
}

Value Interpreter::Visit(const VarNode& node) {
    if (!var_table_.contains(node.GetVarName()))
        throw std::logic_error("Not declared variable was used\n");

    return var_table_[node.GetVarName()];
}

Value Interpreter::Visit(const ConstNode& node) {
    return node.GetValue();
}

Value Interpreter::Visit(const BinOpNode& node) {
    const Value& lhs = node.GetLhs().Accept(*this);
    const Value& rhs = node.GetRhs().Accept(*this);

    return bin_op_executor_.ExecuteBinOp(node.GetOperation(), lhs, rhs);
}

void Interpreter::Visit(const SkipNode& node) {
    return;
}

void Interpreter::Visit(const SeqNode& node) {
    node.GetLhs().Accept(*this);
    node.GetRhs().Accept(*this);
}

void Interpreter::Visit(const AssignNode& node) {
    Value& lhs = node.GetLhs().AcceptRef(*this);
    const Value& rhs = node.GetRhs().Accept(*this);

    lhs = rhs;
}

void Interpreter::Visit(const ReadNode& node) {
    try {
        node.GetTargetNode().AcceptRef(*this) = std::stoll(input_provider_->GetInput());
    } catch (std::invalid_argument exception) {
        throw std::invalid_argument("Input is not float constant");
    } catch (std::out_of_range exception) {
        throw std::invalid_argument("Float constant from input is too big");
    }
}

void Interpreter::Visit(const WriteNode& node) {
    output_provider_->Write(std::to_string(node.GetExprNode().Accept(*this)) + " ");
}

void Interpreter::Visit(const WhileNode& node) {
    while (node.GetCondition().Accept(*this)) {
        node.GetBody().Accept(*this);
    }
}

void Interpreter::Visit(const DoWhileNode& node) {
    do {
        node.GetBody().Accept(*this);
    } while (node.GetCondition().Accept(*this));
}

void Interpreter::Visit(const IfNode& node) {
    if (node.GetCondition().Accept(*this)) {
        node.GetIfVariant().Accept(*this);
    }
    else {
        node.GetElseVariant().Accept(*this);
    }
}