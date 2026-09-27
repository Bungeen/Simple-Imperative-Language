#pragma once

#include <memory>

#include "input_provider.h"
#include "output_provider.h"
#include "bin_op_executor.h"

class Const;
class Var;
class BinOpExpr;
class SkipStmt;
class SeqStmt;
class AssignStmt;
class ReadStmt;
class WriteStmt;
class WhileStmt;
class DoWhileStmt;
class IfStmt;

class Interpreter {
public:
    Interpreter(std::unique_ptr<InputProvider> input_provider,
                std::unique_ptr<OutputProvider> output_provider);

    Value Visit(const Var& node);
    Value Visit(const Const& node);
    Value Visit(const BinOpExpr& node);

    void Visit(const SkipStmt& node);
    void Visit(const SeqStmt& node);
    void Visit(const AssignStmt& node);
    void Visit(const ReadStmt& node);
    void Visit(const WriteStmt& node);
    void Visit(const WhileStmt& node);
    void Visit(const DoWhileStmt& node);
    void Visit(const IfStmt& node);
private:
    std::unordered_map<std::string, Value> var_table_;
    std::unique_ptr<InputProvider> input_provider_;
    std::unique_ptr<OutputProvider> output_provider_;
    BinOpExecutor bin_op_executor_;
};