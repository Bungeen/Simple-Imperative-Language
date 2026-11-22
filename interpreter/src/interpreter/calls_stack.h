#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>

#include "value.h"

class Stmt;

struct FunctionDef {
    std::string name;
    std::vector<std::string> params;
    std::shared_ptr<Stmt> body;
};

class CallsStack {
public:
    using Scope = std::unordered_map<std::string, Value>;
    using FunctionCall = std::vector<Scope>;
    using FunctionTable = std::unordered_map<std::string, FunctionDef>;

    CallsStack();

    Value& Get(const std::string& id);
    bool Contains(const std::string& id);
    void AddFunction();
    void AddScope();
    void SetValueForScope(const std::string& id, Value value);
    void SetValueForFunction(const std::string& id, Value value);
    void RemoveScope();
    void RemoveFunction();

    void AddFunctionDef(FunctionDef func);
    const FunctionDef& GetFunctionDef(const std::string& name) const;
    bool HasFunctionDef(const std::string& name) const;
private:
    std::vector<FunctionCall> data_;
    std::vector<std::vector<FunctionTable>> function_tables_;
};
