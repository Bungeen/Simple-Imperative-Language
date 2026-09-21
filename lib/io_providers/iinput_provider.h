#pragma once

#include <string>

class IInputProvider {
public:
    virtual ~IInputProvider() = default;
    virtual std::string GetInput() = 0;
};