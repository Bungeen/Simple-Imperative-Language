#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "ast_json.h"
#include "interpreter.h"
#include "cli_input_provider.h"
#include "cli_output_provider.h"

int main(int argc, char* argv[]) {
    bool run = argc >= 2 && std::string(argv[1]) == "--run";

    if ((run && argc != 3) || (!run && argc > 2)) {
        std::cerr << "Usage: ast_json [file.json]\n";
        std::cerr << "       ast_json --run file.json\n";
        return 1;
    }

    try {
        StmtPtr program;

        if (argc >= 2) {
            const char* filename = run ? argv[2] : argv[1];
            std::ifstream input(filename, std::ios::binary);

            if (!input) {
                throw std::runtime_error(std::string("cannot open file: ") + filename);
            }

            program = ast_json::read_program(input);
        } else {
            program = ast_json::read_program(std::cin);
        }

        if (run) {
            Interpreter interpreter(std::make_unique<CliInputProvider>(), 
                                    std::make_unique<CliOutputProvider>());
            program->execute(interpreter);
            return 0;
        }

        program->show(std::cout);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << std::endl;
        return 1;
    }

    return 0;
}
