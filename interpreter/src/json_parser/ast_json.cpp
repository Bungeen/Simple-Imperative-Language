#include "ast_json.h"
#include "json_format.h"

#include <nlohmann/json.hpp>

#include <istream>
#include <stdexcept>
#include <initializer_list>
#include <iterator>
#include <limits>


namespace ast_json {
using Json = nlohmann::json;

namespace {

[[noreturn]] void fail(const std::string& path, const std::string& message) {
    throw std::invalid_argument(path + ": " + message);
}

std::string field_path(const std::string& path, const std::string& name) {
    return path + json_format::PATH_SEPARATOR + name;
}

Json parse_json(const std::string& text) {
    return Json::parse(text);
}

void check_object(const Json& value, const std::string& path) {
    if (!value.is_object()) {
        fail(path, "expected an object");
    }
}

void check_fields(const Json& value, const std::string& path,
                    std::initializer_list<std::string> required,
                    std::initializer_list<std::string> optional = {}) {

    check_object(value, path);

    for (const std::string& name : required) {
        if (!value.contains(name)) {
            fail(path, std::string("missing field: ") + name);
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

std::string identifier(const Json& value, const std::string& path) {
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
        fail(path, "identifire can't start with no lower leter");
    }

    for (char c : name) {
        bool is_letter = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        bool is_digit = c >= '0' && c <= '9';
        bool is_special = c == '_' || c == '\'';

        if (!is_letter && !is_digit && !is_special) {
            fail(path, "invalid character in identifier");
        }
    }

    return name;
}

std::int64_t constant(const Json& value, const std::string& path) {
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

BinOp operation(const Json& value, const std::string& path) {
    if (!value.is_string()) {
        fail(path, "expected a bin operation string");
    }

    std::string text = value.get<std::string>();

    for (BinOp op : ALL_BINOPS) {
        if (text == spelling(op)) {
            return op;
        }
    }

    fail(path, "unknown binary operator: " + text);
}

ExprPtr expression(const Json& value, const std::string& path) {
    check_object(value, path);

    if (value.contains(json_format::CONST)) {
        check_fields(value, path, {json_format::CONST});

        std::int64_t number = constant(value.at(json_format::CONST), field_path(path, json_format::CONST));

        return std::make_unique<Const>(number);
    }

    if (value.contains(json_format::VAR)) {
        check_fields(value, path, {json_format::VAR});

        std::string name = identifier(value.at(json_format::VAR), field_path(path, json_format::VAR));

        return std::make_unique<Var>(name);
    }

    if (value.contains(json_format::BINOP)) {
        check_fields(value, path, {json_format::BINOP, json_format::LEFT, json_format::RIGHT});

        BinOp op = operation(value.at(json_format::BINOP), field_path(path, json_format::BINOP));
        ExprPtr left = expression(value.at(json_format::LEFT), field_path(path, json_format::LEFT));
        ExprPtr right = expression(value.at(json_format::RIGHT), field_path(path, json_format::RIGHT));

        return std::make_unique<BinOpExpr>(op, std::move(left), std::move(right));
    }

    fail(path, "unknown expression; expected const, var or binop");
}

StmtPtr statement(const Json& value, const std::string& path) {
    check_object(value, path);

    if (value.size() != 1) {
        fail(path, "a statement must have exactly one node tag");
    }

    std::string tag = value.begin().key();
    const Json& body = value.at(tag);
    std::string here = field_path(path, tag);

    if (tag == json_format::READ) {
        std::string name = identifier(body, here);

        return std::make_unique<ReadStmt>(name);
    }

    if (tag == json_format::WRITE) {
        ExprPtr expr = expression(body, here);

        return std::make_unique<WriteStmt>(std::move(expr));
    }

    if (tag == json_format::SKIP) {
        if (!body.is_null()) {
            fail(here, "expected null");
        }

        return std::make_unique<SkipStmt>();
    }

    if (tag == json_format::SEQ) {
        check_fields(body, here, {json_format::LEFT, json_format::RIGHT});

        StmtPtr left = statement(body.at(json_format::LEFT), field_path(here, json_format::LEFT));
        StmtPtr right = statement(body.at(json_format::RIGHT), field_path(here, json_format::RIGHT));

        return std::make_unique<SeqStmt>(std::move(left), std::move(right));
    }

    if (tag == json_format::ASSN) {
        check_fields(body, here, {json_format::DST, json_format::SRC});

        std::string dst = identifier(body.at(json_format::DST), field_path(here, json_format::DST));
        ExprPtr src = expression(body.at(json_format::SRC), field_path(here, json_format::SRC));

        return std::make_unique<AssignStmt>(dst, std::move(src));
    }

    if (tag == json_format::WHILE) {
        check_fields(body, here, {json_format::COND, json_format::BODY});

        ExprPtr cond = expression(body.at(json_format::COND), field_path(here, json_format::COND));
        StmtPtr loop_body = statement(body.at(json_format::BODY), field_path(here, json_format::BODY));

        return std::make_unique<WhileStmt>(std::move(cond), std::move(loop_body));
    }

    if (tag == json_format::DO_WHILE) {
        check_fields(body, here, {json_format::BODY, json_format::COND});

        StmtPtr loop_body = statement(body.at(json_format::BODY), field_path(here, json_format::BODY));
        ExprPtr cond = expression(body.at(json_format::COND), field_path(here, json_format::COND));

        return std::make_unique<DoWhileStmt>(std::move(loop_body), std::move(cond));
    }

    if (tag == json_format::IF) {
        check_fields(body, here, {json_format::COND, json_format::THEN, json_format::ELSE});

        ExprPtr cond = expression(body.at(json_format::COND), field_path(here, json_format::COND));
        StmtPtr then_branch = statement(body.at(json_format::THEN), field_path(here, json_format::THEN));
        StmtPtr else_branch = statement(body.at(json_format::ELSE), field_path(here, json_format::ELSE));

        return std::make_unique<IfStmt>(
            std::move(cond),
            std::move(then_branch),
            std::move(else_branch)
        );
    }

    fail(here, "unknown statement tag");
}
}



ExprPtr expr_from_json(const std::string& text) {
    Json value = parse_json(text);

    return expression(value, json_format::ROOT_PATH);
}

StmtPtr stmt_from_json(const std::string& text) {
    Json value = parse_json(text);

    return statement(value, json_format::ROOT_PATH);
}

StmtPtr read_program(std::istream& input) {
    std::string text{
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()
    };

    if (input.bad()) {
        throw std::runtime_error("failed to read JSON input");
    }

    return stmt_from_json(text);
}

}
