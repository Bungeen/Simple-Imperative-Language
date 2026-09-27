#pragma once

#include <iostream>
#include <string>

#include "input_provider.h"

class CliInputProvider : public InputProvider {
public:
    std::string get_input() override {
        std::string data{};
        std::cin >> data;

        return data;
    }
};