#include <stdexcept>

#include "calls_stack.h"

CallsStack::CallsStack()
    : data_(1, FunctionCall())
{
    data_[0].push_back(Scope{});
}

Value& CallsStack::Get(const std::string& id) {
    std::vector<Scope>::iterator iter = data_.back().end();
    do {
        --iter;
        if (iter->contains(id)) {
            return iter->at(id);
        }
    } while (iter != data_.back().begin());

    throw std::logic_error("tried to use undeclared variable");
}

bool CallsStack::Contains(const std::string& id) {
    std::vector<Scope>::iterator iter = data_.back().end();
    do {
        --iter;
        if (iter->contains(id)) {
            return true;
        }
    } while (iter != data_.back().begin());

    return false;
}

void CallsStack::AddFunction() {
    data_.push_back(FunctionCall());
    AddScope();
}

void CallsStack::AddScope() {
    data_.back().push_back(Scope());
}

void CallsStack::SetValueForFunction(const std::string& id, Value value) {
    data_.back()[0][id] = std::move(value);
}

void CallsStack::SetValueForScope(const std::string& id, Value value) {
    data_.back().back()[id] = std::move(value);
}

void CallsStack::RemoveScope() {
    data_.back().pop_back();
}

void CallsStack::RemoveFunction() {
    data_.pop_back();
}