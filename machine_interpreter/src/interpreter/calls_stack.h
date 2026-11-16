#pragma once

#include <vector>
#include <unordered_map>
#include <iostream>

#include "value.h"

class CallsStack {
public:
    using Scope = std::unordered_map<std::string, Value>;
    using FunctionCall = std::vector<Scope>;

    CallsStack();

    Value& Get(const std::string& id);
    bool Contains(const std::string& id);
    void AddFunction();
    void AddScope();
    void SetValueForScope(const std::string& id, Value value);
    void SetValueForFunction(const std::string& id, Value value);
    void RemoveScope();
    void RemoveFunction();
private:
    std::vector<FunctionCall> data_;
};