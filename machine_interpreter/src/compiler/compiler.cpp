#include "compiler.h"

#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

static const std::unordered_map<std::string, BinOp> kBinOpMap{
    {"!!", BinOp::Or},
    {"&&", BinOp::And},
    {"==", BinOp::Equal},
    {"!=", BinOp::NotEqual},
    {"<=", BinOp::LessEqual},
    {">=", BinOp::GreaterEqual},
    {">", BinOp::Greater},
    {"<", BinOp::Less},
    {"+", BinOp::Add},
    {"-", BinOp::Subtract},
    {"*", BinOp::Multiply},
    {"/", BinOp::Divide},
    {"%", BinOp::Modulo}
};

[[noreturn]] static void fail(const std::string& path, const std::string& message) {
    throw std::invalid_argument(path + ": " + message);
}

static std::string field_path(const std::string& path, const std::string& name) {
    return path + "." + name;
}

static void check_object(const nlohmann::json& value, const std::string& path) {
    if (!value.is_object()) {
        fail(path, "expected an object");
    }
}

static void check_fields(const nlohmann::json& value, const std::string& path,
                         std::initializer_list<std::string> required,
                         std::initializer_list<std::string> optional = {}) {
    check_object(value, path);

    for (const std::string& name : required) {
        if (!value.contains(name)) {
            fail(path, "missing field: " + name);
        }
    }

    for (const auto& field : value.items()) {
        bool known = false;

        for (const std::string& name : required) {
            if (field.key() == name) {
                known = true;
                break;
            }
        }

        for (const std::string& name : optional) {
            if (field.key() == name) {
                known = true;
                break;
            }
        }

        if (!known) {
            fail(path, "unknown field: " + field.key());
        }
    }
}

static std::string identifier(const nlohmann::json& value, const std::string& path) {
    if (!value.is_string()) {
        fail(path, "expected an identifier string");
    }

    std::string name = value.get<std::string>();

    if (name.empty()) {
        fail(path, "identifier can't be empty");
    }

    char c = name.front();
    bool is_letter = (c >= 'a' && c <= 'z');
    if (!is_letter) {
        fail(path, "identifier can't start with non-lowercase letter");
    }

    for (char ch : name) {
        bool is_valid = (ch >= 'a' && ch <= 'z') ||
                        (ch >= 'A' && ch <= 'Z') ||
                        (ch >= '0' && ch <= '9') ||
                        ch == '_' || ch == '\'';
        if (!is_valid) {
            fail(path, "invalid character in identifier");
        }
    }

    return name;
}

static std::int64_t constant(const nlohmann::json& value, const std::string& path) {
    if (!value.is_number_integer()) {
        fail(path, "expected an int64 integer");
    }

    if (value.is_number_unsigned()) {
        std::uint64_t number = value.get<std::uint64_t>();
        std::uint64_t maximum = std::numeric_limits<std::int64_t>::max();

        if (number > maximum) {
            fail(path, "constant is outside the int64 range");
        }
    }

    return value.get<std::int64_t>();
}

static BinOp operation(const nlohmann::json& value, const std::string& path) {
    if (!value.is_string()) {
        fail(path, "expected a bin operation string");
    }

    std::string text = value.get<std::string>();
    auto it = kBinOpMap.find(text);
    if (it != kBinOpMap.end()) {
        return it->second;
    }

    fail(path, "unknown binary operator: " + text);
}

static void compile_expression(const nlohmann::json& value, const std::string& path,
                               std::vector<Command>& commands) {
    check_object(value, path);

    if (value.contains("const")) {
        check_fields(value, path, {"const"});
        std::int64_t number = constant(value.at("const"), field_path(path, "const"));
        commands.push_back(Command{Operation::Const, number});
        return;
    }

    if (value.contains("var")) {
        check_fields(value, path, {"var"});
        std::string name = identifier(value.at("var"), field_path(path, "var"));
        commands.push_back(Command{Operation::Ld, name});
        return;
    }

    if (value.contains("binop")) {
        check_fields(value, path, {"binop", "left", "right"});
        BinOp op = operation(value.at("binop"), field_path(path, "binop"));
        compile_expression(value.at("left"), field_path(path, "left"), commands);
        compile_expression(value.at("right"), field_path(path, "right"), commands);
        commands.push_back(Command{Operation::Binop, op});
        return;
    }

    fail(path, "unknown expression; expected const, var or binop");
}

static void compile_statement(const nlohmann::json& value, const std::string& path,
                              std::vector<Command>& commands, std::size_t& next_label) {
    check_object(value, path);

    if (value.size() != 1) {
        fail(path, "a statement must have exactly one node tag");
    }

    std::string tag = value.begin().key();
    const auto& body = value.at(tag);
    std::string here = field_path(path, tag);

    if (tag == "read") {
        std::string name = identifier(body, here);
        commands.push_back(Command{Operation::Read, CommandArgument()});
        commands.push_back(Command{Operation::St, name});
        return;
    }

    if (tag == "write") {
        compile_expression(body, here, commands);
        commands.push_back(Command{Operation::Write, CommandArgument()});
        return;
    }

    if (tag == "skip") {
        if (!body.is_null()) {
            fail(here, "expected null");
        }
        return;
    }

    if (tag == "seq") {
        check_fields(body, here, {"left", "right"});
        compile_statement(body.at("left"), field_path(here, "left"), commands, next_label);
        compile_statement(body.at("right"), field_path(here, "right"), commands, next_label);
        return;
    }

    if (tag == "assn") {
        check_fields(body, here, {"dst", "src"});
        std::string dst = identifier(body.at("dst"), field_path(here, "dst"));
        compile_expression(body.at("src"), field_path(here, "src"), commands);
        commands.push_back(Command{Operation::St, dst});
        return;
    }

    if (tag == "while") {
        check_fields(body, here, {"cond", "body"});

        std::string body_lbl = "L_" + std::to_string(next_label++);
        std::string test_lbl = "L_" + std::to_string(next_label++);

        commands.push_back(Command{Operation::Jmp, test_lbl});
        commands.push_back(Command{Operation::Label, body_lbl});
        compile_statement(body.at("body"), field_path(here, "body"), commands, next_label);
        commands.push_back(Command{Operation::Label, test_lbl});
        compile_expression(body.at("cond"), field_path(here, "cond"), commands);
        commands.push_back(Command{Operation::Jnz, body_lbl});
        return;
    }

    if (tag == "do_while") {
        check_fields(body, here, {"body", "cond"});

        std::string start_lbl = "L_" + std::to_string(next_label++);

        commands.push_back(Command{Operation::Label, start_lbl});
        compile_statement(body.at("body"), field_path(here, "body"), commands, next_label);
        compile_expression(body.at("cond"), field_path(here, "cond"), commands);
        commands.push_back(Command{Operation::Jnz, start_lbl});
        return;
    }

    if (tag == "if") {
        check_fields(body, here, {"cond", "then", "else"});

        std::string else_lbl = "L_" + std::to_string(next_label++);
        std::string end_lbl = "L_" + std::to_string(next_label++);

        compile_expression(body.at("cond"), field_path(here, "cond"), commands);
        commands.push_back(Command{Operation::Jz, else_lbl});
        compile_statement(body.at("then"), field_path(here, "then"), commands, next_label);
        commands.push_back(Command{Operation::Jmp, end_lbl});
        commands.push_back(Command{Operation::Label, else_lbl});
        compile_statement(body.at("else"), field_path(here, "else"), commands, next_label);
        commands.push_back(Command{Operation::Label, end_lbl});
        return;
    }

    fail(here, "unknown statement tag");
}

std::vector<Command> compile(const nlohmann::json& input) {
    std::vector<Command> commands;
    std::size_t next_label = 0;
    compile_statement(input, "$", commands, next_label);
    return commands;
}
