#include <stdexcept>

#include "calls_stack.h"
#include "stmt.h"

CallsStack::CallsStack()
    : data_(1, FunctionCall())
    , function_tables_(1, std::vector<FunctionTable>())
{
    data_[0].push_back(Scope{});
    function_tables_[0].push_back(FunctionTable{});
}

Value& CallsStack::Get(const std::string& id) {
    for (auto call_it = data_.rbegin(); call_it != data_.rend(); ++call_it) {
        for (auto scope_it = call_it->rbegin(); scope_it != call_it->rend(); ++scope_it) {
            if (scope_it->contains(id)) {
                return scope_it->at(id);
            }
        }
    }

    throw std::logic_error("tried to use undeclared variable: " + id);
}

bool CallsStack::Contains(const std::string& id) {
    for (auto call_it = data_.rbegin(); call_it != data_.rend(); ++call_it) {
        for (auto scope_it = call_it->rbegin(); scope_it != call_it->rend(); ++scope_it) {
            if (scope_it->contains(id)) {
                return true;
            }
        }
    }

    return false;
}

void CallsStack::AddFunction() {
    data_.push_back(FunctionCall());
    function_tables_.push_back(std::vector<FunctionTable>());
    AddScope();
}

void CallsStack::AddScope() {
    data_.back().push_back(Scope());
    function_tables_.back().push_back(FunctionTable());
}

void CallsStack::SetValueForFunction(const std::string& id, Value value) {
    for (auto call_it = data_.rbegin(); call_it != data_.rend(); ++call_it) {
        for (auto scope_it = call_it->rbegin(); scope_it != call_it->rend(); ++scope_it) {
            if (scope_it->contains(id)) {
                scope_it->at(id) = std::move(value);
                return;
            }
        }
    }

    data_.back()[0][id] = std::move(value);
}

void CallsStack::SetValueForScope(const std::string& id, Value value) {
    data_.back().back()[id] = std::move(value);
}

void CallsStack::RemoveScope() {
    data_.back().pop_back();
    function_tables_.back().pop_back();
}

void CallsStack::RemoveFunction() {
    data_.pop_back();
    function_tables_.pop_back();
}

void CallsStack::AddFunctionDef(FunctionDef func) {
    std::string name = func.name;
    function_tables_.back().back()[std::move(name)] = std::move(func);
}

bool CallsStack::HasFunctionDef(const std::string& name) const {
    for (auto call_it = function_tables_.rbegin(); call_it != function_tables_.rend(); ++call_it) {
        for (auto scope_it = call_it->rbegin(); scope_it != call_it->rend(); ++scope_it) {
            if (scope_it->contains(name)) {
                return true;
            }
        }
    }

    return false;
}

const FunctionDef& CallsStack::GetFunctionDef(const std::string& name) const {
    for (auto call_it = function_tables_.rbegin(); call_it != function_tables_.rend(); ++call_it) {
        for (auto scope_it = call_it->rbegin(); scope_it != call_it->rend(); ++scope_it) {
            if (scope_it->contains(name)) {
                return scope_it->at(name);
            }
        }
    }

    throw std::logic_error("tried to call undeclared function: " + name);
}
