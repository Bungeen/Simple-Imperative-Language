#include <stdexcept>
#include <nlohmann/json.hpp>

#include "program_json.h"
#include "command.h"

std::vector<Command> parse(const nlohmann::json& input) {
    std::vector<Command> program{};

    for (const auto& elem : input) {
        if (elem.is_string()) {
            if (elem == "READ") {
                program.push_back(Command{Operation::Read, CommandArgument()});
            } else if (elem == "WRITE") {
                program.push_back(Command{Operation::Write, CommandArgument()});
            } 
        } else if (elem.is_object()) {
            const auto& key = elem.begin().key();
            const auto& value = elem.begin().value();

            if (key == "LD") {
                program.push_back(Command{Operation::Ld, CommandArgument(value.get<std::string>())});
            } else if (key == "ST") {
                program.push_back(Command{Operation::St, CommandArgument(value.get<std::string>())});
            } else if (key == "CONST") {
                program.push_back(Command{Operation::Const, CommandArgument(value.get<std::int64_t>())});
            } else if (key == "BINOP") {
                program.push_back(Command{Operation::Binop, CommandArgument(kBinOpMap.at(value.get<std::string>()))});
            } else if (key == "LABEL") {
                program.push_back(Command{Operation::Label, CommandArgument(value.get<std::string>())});
            } else if (key == "JMP") {
                program.push_back(Command{Operation::Jmp, CommandArgument(value.get<std::string>())});
            } else if (key == "JZ") {
                program.push_back(Command{Operation::Jz, CommandArgument(value.get<std::string>())});
            } else if (key == "JNZ") {
                program.push_back(Command{Operation::Jnz, CommandArgument(value.get<std::string>())});
            }
        } else {
            throw std::invalid_argument("invalid json input");
        }
    }

    return program;
}