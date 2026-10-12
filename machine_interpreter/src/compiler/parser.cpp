#include "parser.h"
#include "compiler_error.h"

#include <charconv>
#include <iterator>
#include <utility>

Parser::Parser(std::string_view source) 
    : lexer_(source)
    , token_(lexer_.next())
{}

std::vector<Command> Parser::program() {
    block();

    if (token_.kind != Kind::End) {
        fail(token_, "Unexpected text after program");
    }

    return std::move(commands_);
}

void Parser::advance() {
    token_ = lexer_.next();
}

bool Parser::accept(std::string_view text) {
    if (token_.text != text) {
        return false;
    }

    advance();
    return true;
}

void Parser::expect(std::string_view text) {
    if (!accept(text)) {
        fail(token_, "Expected '" + std::string(text) + "'");
    }
}

std::string Parser::identifier() {
    if (token_.kind != Kind::Identifier) {
        fail(token_, "Expected identifier");
    }

    std::string name(token_.text);
    advance();
    return name;
}

void Parser::emit(Operation operation, CommandArgument argument) {
    commands_.push_back({operation, std::move(argument)});
}

std::string Parser::label() {
    return "L_" + std::to_string(next_label_++);
}

const Operator *Parser::binary_operator() const {
    for (const auto &op : kOperators) {
        if (op.text == token_.text) {
            return &op;
        }
    }

    return nullptr;
}

void Parser::operand() {
    if (token_.kind == Kind::Identifier) {
        emit(Operation::Ld, identifier());
        return;
    }

    if (token_.kind == Kind::Number) {
        Value value{};
        const auto result = std::from_chars(token_.text.data(), token_.text.data() + token_.text.size(), value);

        if (result.ec != std::errc{}) {
            fail(token_, "Integer constant is out of range");
        }

        emit(Operation::Const, value);
        advance();
        return;
    }

    if (accept("(")) {
        expression();
        expect(")");
        return;
    }

    fail(token_, "Expected expression");
}

void Parser::expression(int minimum_precedence) {
    operand();

    const Operator *op = binary_operator();
    while (op != nullptr && op->precedence >= minimum_precedence) {
        advance();
        expression(op->precedence + 1);
        emit(Operation::Binop, op->operation);
        op = binary_operator();
    }
}

void Parser::condition() {
    expect("(");
    expression();
    expect(")");
}

void Parser::block() {
    expect("{");

    if (token_.text == "}") {
        fail(token_, "Block must contain at least one statement; use skip");
    }

    while (token_.text != "}") {
        if (token_.kind == Kind::End) {
            fail(token_, "Expected '}'");
        }

        statement();
    }

    expect("}");
}

void Parser::conditional() {
    const auto otherwise = label();
    const auto end = label();

    condition();
    emit(Operation::Jz, otherwise);
    statement();

    emit(Operation::Jmp, end);
    emit(Operation::Label, otherwise);
    if (accept("else")) {
        statement();
        emit(Operation::Label, end);
        return;
    }

    if (accept("elif")) {
        conditional();
    }

    emit(Operation::Label, end);
}

void Parser::statement() {
    if (token_.text == "{") {
        block();
        return;
    }

    if (accept("read")) {
        expect("(");
        const auto name = identifier();
        expect(")");
        emit(Operation::Read);
        emit(Operation::St, name);
        accept(";");
        return;
    }

    if (accept("write")) {
        condition();
        emit(Operation::Write);
        accept(";");
        return;
    }

    if (accept("skip")) {
        accept(";");
        return;
    }

    if (accept("if")) {
        conditional();
        return;
    }

    if (accept("while")) {
        const auto body = label();
        const auto test = label();

        const auto cond_start = commands_.size();
        condition();
        std::vector<Command> cond_cmds(
            std::make_move_iterator(commands_.begin() + cond_start),
            std::make_move_iterator(commands_.end())
        );
        commands_.erase(commands_.begin() + cond_start, commands_.end());

        emit(Operation::Jmp, test);
        emit(Operation::Label, body);
        statement();
        emit(Operation::Label, test);
        commands_.insert(
            commands_.end(),
            std::make_move_iterator(cond_cmds.begin()),
            std::make_move_iterator(cond_cmds.end())
        );
        emit(Operation::Jnz, body);
        return;
    }

    if (accept("do")) {
        const auto start = label();
        emit(Operation::Label, start);
        statement();
        expect("while");
        condition();
        emit(Operation::Jnz, start);
        accept(";");
        return;
    }

    if (accept("for")) {
        const auto body = label();
        const auto test = label();

        expect("(");
        statement();

        const auto cond_start = commands_.size();
        expression();
        expect(";");
        std::vector<Command> cond_cmds(
            std::make_move_iterator(commands_.begin() + cond_start),
            std::make_move_iterator(commands_.end())
        );
        commands_.erase(commands_.begin() + cond_start, commands_.end());

        const auto step_start = commands_.size();
        statement();
        expect(")");
        std::vector<Command> step_cmds(
            std::make_move_iterator(commands_.begin() + step_start),
            std::make_move_iterator(commands_.end())
        );
        commands_.erase(commands_.begin() + step_start, commands_.end());

        emit(Operation::Jmp, test);
        emit(Operation::Label, body);
        statement();
        commands_.insert(
            commands_.end(),
            std::make_move_iterator(step_cmds.begin()),
            std::make_move_iterator(step_cmds.end())
        );
        emit(Operation::Label, test);
        commands_.insert(
            commands_.end(),
            std::make_move_iterator(cond_cmds.begin()),
            std::make_move_iterator(cond_cmds.end())
        );
        emit(Operation::Jnz, body);
        return;
    }

    if (token_.kind == Kind::Identifier) {
        const auto name = identifier();
        const auto *op = binary_operator();
        if (op) {
            advance();
            emit(Operation::Ld, name);
        }
        expect("=");
        expression();
        if (op) {
            emit(Operation::Binop, op->operation);
        }
        emit(Operation::St, name);
        accept(";");
        return;
    }

    fail(token_, "Expected statement");
}
