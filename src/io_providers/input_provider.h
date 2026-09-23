#pragma once

#include <string>

class InputProvider {
public:
    virtual ~InputProvider() = default;
    virtual std::string GetInput() = 0;
};