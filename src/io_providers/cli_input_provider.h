#pragma once

#include <iostream>
#include <string>

#include "input_provider.h"

class CliInputProvider : public InputProvider {
public:
    std::string GetInput() override {
        std::string data{};
        std::cin >> data;

        return data;
    }
};