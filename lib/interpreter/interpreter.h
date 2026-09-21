#pragma once

#include <memory>

#include "inode_visitor.h"
#include "iinput_provider.h"
#include "ioutput_provider.h"
#include "bin_op_executor.h"
#include "ast.h"

class Interpreter : public INodeVisitor {
public:
    Interpreter(std::unique_ptr<IInputProvider> input_provider,
                std::unique_ptr<IOutputProvider> output_provider);

    Value& VisitLvalue(const VarNode& node) override;

    Value Visit(const VarNode& node) override;
    Value Visit(const ConstNode& node) override;
    Value Visit(const BinOpNode& node) override;

    void Visit(const SkipNode& node) override;
    void Visit(const SeqNode& node) override;
    void Visit(const AssignNode& node) override;
    void Visit(const ReadNode& node) override;
    void Visit(const WriteNode& node) override;
    void Visit(const WhileNode& node) override;
    void Visit(const DoWhileNode& node) override;
    void Visit(const IfNode& node) override;
private:
    std::unordered_map<std::string, Value> var_table_;
    std::unique_ptr<IInputProvider> input_provider_;
    std::unique_ptr<IOutputProvider> output_provider_;
    BinOpExecutor bin_op_executor_;
};