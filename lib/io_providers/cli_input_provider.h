#pragma once

#include <iostream>
#include <string>

#include "iinput_provider.h"

class CliInputProvider : public IInputProvider {
public:
    std::string GetInput() override {
        std::string data{};
        std::cin >> data;

        return data;
    }
};