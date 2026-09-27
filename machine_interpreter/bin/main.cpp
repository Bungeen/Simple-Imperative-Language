#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>

#include "command.h"
#include "program_json.h"
#include "interpreter.h"
#include "cli_input_provider.h"
#include "cli_output_provider.h"

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "filename required" << std::endl;

        return 0;
    }
        
    std::ifstream fin(argv[1]);
    
    if (!fin.is_open()) {
        std::cout << "Didn't managed to open specified file";

        return 0;
    }

    nlohmann::json program{};

    fin >> program;

    std::vector<Command> parsed_program = parse(program);

    Interpreter interpreter(std::make_unique<CliInputProvider>(), 
                            std::make_unique<CliOutputProvider>());

    interpreter.interpret(parsed_program);

    return 0;
}