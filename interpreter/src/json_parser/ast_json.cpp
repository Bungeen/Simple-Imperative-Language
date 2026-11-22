#include "ast_json.h"
#include "json_format.h"

#include <nlohmann/json.hpp>

#include <istream>
#include <stdexcept>
#include <initializer_list>
#include <iterator>
#include <limits>

#include <iostream>

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

    if (value.contains(json_format::VAR)) {
        check_fields(value, path, {json_format::VAR});

        std::string name = identifier(value.at(json_format::VAR), field_path(path, json_format::VAR));

        return std::make_unique<Var>(name);
    }

    if (value.contains(json_format::ELEM) && value.contains(json_format::INDEX)) {
        check_fields(value, path, {json_format::ELEM, json_format::INDEX});

        ExprPtr list = expression(value.at(json_format::ELEM), field_path(path, json_format::ELEM));
        ExprPtr index = expression(value.at(json_format::INDEX), field_path(path, json_format::INDEX));

        return std::make_unique<ElemAccess>(std::move(list), std::move(index));
    }

    if (value.contains(json_format::CONST)) {
        check_fields(value, path, {json_format::CONST});

        std::int64_t number = constant(value.at(json_format::CONST), field_path(path, json_format::CONST));

        return std::make_unique<Const>(number);
    }

    if (value.contains(json_format::STRING)) {
        check_fields(value, path, {json_format::STRING});

        std::shared_ptr<std::string> string_value = std::make_shared<std::string>(value.at(json_format::STRING));

        return std::make_unique<Const>(string_value);
    }

    if (value.contains(json_format::SEXP)) {
        check_fields(value, path, {json_format::SEXP, json_format::ARGS});

        std::string tag = value.at(json_format::SEXP);
        std::vector<ExprPtr> args{};

        const Json& params = value.at(json_format::ARGS);

        for (std::size_t i = 0; i < params.size(); ++i) {
            std::string elem_path = path + json_format::PATH_SEPARATOR + 
                                    json_format::ARGS +"[" + std::to_string(i) + "]";

            args.push_back(expression(value.at(json_format::ARGS)[i], elem_path));
        }

        return std::make_unique<SexpExpr>(std::move(tag), std::move(args));
    }

    if (value.contains(json_format::BINOP)) {
        check_fields(value, path, {json_format::BINOP, json_format::LEFT, json_format::RIGHT});

        BinOp op = operation(value.at(json_format::BINOP), field_path(path, json_format::BINOP));
        ExprPtr left = expression(value.at(json_format::LEFT), field_path(path, json_format::LEFT));
        ExprPtr right = expression(value.at(json_format::RIGHT), field_path(path, json_format::RIGHT));

        return std::make_unique<BinOpExpr>(op, std::move(left), std::move(right));
    }

    if (value.contains(json_format::LENGTH)) {
        check_fields(value, path, {json_format::LENGTH});

        ExprPtr length = expression(value.at(json_format::LENGTH), field_path(path, json_format::LENGTH));

        return std::make_unique<Length>(std::move(length));
    }

    if (value.contains(json_format::ARRAY)) {
        check_fields(value, path, {json_format::ARRAY});

        std::vector<ExprPtr> result{};

        const Json& params = value.at(json_format::ARRAY);

        for (std::size_t i = 0; i < params.size(); ++i) {
            std::string elem_path = path + json_format::PATH_SEPARATOR + 
                                    json_format::ARRAY +"[" + std::to_string(i) + "]";

            result.push_back(expression(value.at(json_format::ARRAY)[i], elem_path));
        }

        return std::make_unique<ListExpr>(std::move(result));
    }

    if (value.contains(json_format::CALL)) {
        const Json& call_json = value.at(json_format::CALL);
        const std::string call_path = field_path(path, json_format::CALL);
        check_object(call_json, call_path);

        std::string name;
        for (const auto& key : {json_format::FUN, json_format::NAME, json_format::VAR}) {
            if (call_json.contains(key)) {
                name = identifier(call_json.at(key), field_path(call_path, key));
                break;
            }
        }

        if (name.empty()) {
            fail(call_path, "missing function name in call");
        }

        std::vector<ExprPtr> args;
        std::string args_key = call_json.contains(json_format::ARGS) ? json_format::ARGS : json_format::PARAMS;
        if (call_json.contains(args_key)) {
            const Json& args_json = call_json.at(args_key);
            if (!args_json.is_array()) {
                fail(field_path(call_path, args_key), "expected array of arguments");
            }

            for (std::size_t i = 0; i < args_json.size(); ++i) {
                std::string arg_path = field_path(call_path, args_key) + "[" + std::to_string(i) + "]";
                args.push_back(expression(args_json[i], arg_path));
            }
        }

        return std::make_unique<CallExpr>(std::move(name), std::move(args));
    }

    fail(path, "unknown expression; expected var, elem access, const, var, elem access or binop");
}

PatternPtr parse_pattern(const Json& value, const std::string& path) {
    if (value.is_number()) {
        std::int64_t number = constant(value, path);

        return std::make_unique<IntPattern>(number);
    }

    if (value.is_string()) {
        return std::make_unique<StringPattern>(value);
    }

    if (value.contains(json_format::WILDCARD)) {
        return std::make_unique<WildcardPattern>();
    }

    if (value.contains(json_format::BIND)) {
        check_fields(value.at(json_format::BIND), 
                     field_path(path, json_format::BIND), 
                     {json_format::PATTERN, json_format::NAME});

        const Json& bind = value.at(json_format::BIND);
        const std::string& bind_path = field_path(path, json_format::BIND);

        std::string name = bind.at(json_format::NAME);
        PatternPtr pattern = parse_pattern(bind.at(json_format::PATTERN), field_path(bind_path, json_format::PATTERN));

        return std::make_unique<BindPattern>(std::move(name), std::move(pattern));
    }

    if (value.contains(json_format::ARRAY)) {
        check_fields(value, path, {json_format::ARRAY});

        std::vector<PatternPtr> result{};

        const Json& params = value.at(json_format::ARRAY);

        for (std::size_t i = 0; i < params.size(); ++i) {
            std::string elem_path = path + json_format::PATH_SEPARATOR + 
                                    json_format::ARRAY +"[" + std::to_string(i) + "]";

            result.push_back(parse_pattern(value.at(json_format::ARRAY)[i], elem_path));
        }

        return std::make_unique<ArrayPattern>(std::move(result));
    }

    if (value.contains(json_format::SEXP)) {
        check_fields(value, path, {json_format::SEXP, json_format::ARGS});

        std::string tag = value.at(json_format::SEXP);
        std::vector<PatternPtr> args{};

        const Json& params = value.at(json_format::ARGS);

        for (std::size_t i = 0; i < params.size(); ++i) {
            std::string elem_path = path + json_format::PATH_SEPARATOR + 
                                    json_format::ARGS +"[" + std::to_string(i) + "]";

            args.push_back(parse_pattern(value.at(json_format::ARGS)[i], elem_path));
        }

        return std::make_unique<SexpPattern>(std::move(tag), std::move(args));
    }

    fail(path, "unknown pattern format");
}

StmtPtr statement(const Json& value, const std::string& path);

StmtPtr parse_var_decl(const Json& value, const std::string& path) {
    if (value.is_string()) {
        return std::make_unique<VarDecl>(identifier(value, path), nullptr);
    }

    check_object(value, path);
    std::string name;
    for (const auto& key : {json_format::NAME, json_format::VAR}) {
        if (value.contains(key)) {
            name = identifier(value.at(key), field_path(path, key));
            break;
        }
    }
    if (name.empty()) {
        fail(path, "missing variable name in var declaration");
    }

    for (const auto& key : {json_format::VALUE, json_format::INIT, json_format::SRC, json_format::EXPR}) {
        if (value.contains(key)) {
            return std::make_unique<VarDecl>(std::move(name), expression(value.at(key), field_path(path, key)));
        }
    }

    return std::make_unique<VarDecl>(std::move(name), nullptr);
}

StmtPtr parse_fun_decl(const Json& value, const std::string& path) {
    check_object(value, path);
    std::string name;
    for (const auto& key : {json_format::NAME, json_format::VAR}) {
        if (value.contains(key)) {
            name = identifier(value.at(key), field_path(path, key));
            break;
        }
    }

    if (name.empty()) {
        fail(path, "missing function name in fun declaration");
    }

    std::vector<std::string> params;
    std::string params_key = value.contains(json_format::ARGS) ? json_format::ARGS : json_format::PARAMS;
    if (value.contains(params_key)) {
        const Json& params_json = value.at(params_key);
        if (!params_json.is_array()) {
            fail(field_path(path, params_key), "expected array of parameter names");
        }

        for (std::size_t i = 0; i < params_json.size(); ++i) {
            std::string p_path = field_path(path, params_key) + "[" + std::to_string(i) + "]";
            params.push_back(identifier(params_json[i], p_path));
        }
    }

    if (!value.contains(json_format::BODY)) {
        fail(path, "missing body in function declaration");
    }

    return std::make_unique<FunDecl>(std::move(name), std::move(params), statement(value.at(json_format::BODY), field_path(path, json_format::BODY)));
}

StmtPtr parse_decl(const Json& value, const std::string& path) {
    check_object(value, path);
    if (value.contains(json_format::NAME) && value.contains(json_format::SPEC)) {
        std::string name = identifier(value.at(json_format::NAME), field_path(path, json_format::NAME));
        const Json& spec = value.at(json_format::SPEC);
        const std::string spec_path = field_path(path, json_format::SPEC);

        if (spec.is_string() && spec.get<std::string>() == json_format::VAR) {
            return std::make_unique<VarDecl>(std::move(name), nullptr);
        }

        if (spec.is_object() && spec.contains(json_format::FUN)) {
            const Json& fun_obj = spec.at(json_format::FUN);
            const std::string fun_path = field_path(spec_path, json_format::FUN);
            check_object(fun_obj, fun_path);

            std::vector<std::string> params;
            std::string params_key = fun_obj.contains(json_format::ARGS) ? json_format::ARGS : json_format::PARAMS;
            if (fun_obj.contains(params_key)) {
                const Json& params_json = fun_obj.at(params_key);

                if (!params_json.is_array()) {
                    fail(field_path(fun_path, params_key), "expected array of parameter names");
                }

                for (std::size_t i = 0; i < params_json.size(); ++i) {
                    std::string p_path = field_path(fun_path, params_key) + "[" + std::to_string(i) + "]";
                    params.push_back(identifier(params_json[i], p_path));
                }
            }

            if (!fun_obj.contains(json_format::BODY)) {
                fail(fun_path, "missing body in function declaration");
            }

            return std::make_unique<FunDecl>(std::move(name), std::move(params), statement(fun_obj.at(json_format::BODY), field_path(fun_path, json_format::BODY)));
        }

        if (spec.is_object() && spec.contains(json_format::VAR)) {
            const Json& var_obj = spec.at(json_format::VAR);
            const std::string var_path = field_path(spec_path, json_format::VAR);
            ExprPtr init = nullptr;
            if (var_obj.is_object()) {
                if (var_obj.contains(json_format::VALUE)) {
                    init = expression(var_obj.at(json_format::VALUE), field_path(var_path, json_format::VALUE));
                } else if (var_obj.contains(json_format::INIT)) {
                    init = expression(var_obj.at(json_format::INIT), field_path(var_path, json_format::INIT));
                } else if (var_obj.contains(json_format::SRC)) {
                    init = expression(var_obj.at(json_format::SRC), field_path(var_path, json_format::SRC));
                } else if (var_obj.contains(json_format::EXPR)) {
                    init = expression(var_obj.at(json_format::EXPR), field_path(var_path, json_format::EXPR));
                }
            }

            return std::make_unique<VarDecl>(std::move(name), std::move(init));
        }
    }

    if (value.contains(json_format::VAR)) {
        const Json& var_content = value.at(json_format::VAR);
        if (var_content.is_array()) {
            if (var_content.empty()) return std::make_unique<SkipStmt>();

            StmtPtr res = parse_var_decl(var_content[0], field_path(path, json_format::VAR) + "[0]");

            for (std::size_t i = 1; i < var_content.size(); ++i) {
                StmtPtr next = parse_var_decl(var_content[i], field_path(path, json_format::VAR) + "[" + std::to_string(i) + "]");
                res = std::make_unique<SeqStmt>(std::move(res), std::move(next));
            }

            return res;
        }

        return parse_var_decl(var_content, field_path(path, json_format::VAR));
    }

    if (value.contains(json_format::FUN)) {
        return parse_fun_decl(value.at(json_format::FUN), field_path(path, json_format::FUN));
    }

    return statement(value, path);
}

std::pair<PatternPtr, StmtPtr> parse_case(const Json& value, const std::string& path) {
    check_object(value, path);

    check_fields(value, path, {json_format::PATTERN, json_format::BRANCH});

    StmtPtr branch = statement(value.at(json_format::BRANCH), field_path(path, json_format::BRANCH));
    PatternPtr pattern = parse_pattern(value.at(json_format::PATTERN), field_path(path, json_format::PATTERN));

    return std::make_pair<PatternPtr, StmtPtr>(std::move(pattern), std::move(branch));
}


StmtPtr statement(const Json& value, const std::string& path) {
    if (value == json_format::SKIP)
        return std::make_unique<SkipStmt>();

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

        std::string dst_name = body.at(json_format::DST);
        ExprPtr src = expression(body.at(json_format::SRC), field_path(here, json_format::SRC));

        return std::make_unique<AssignStmt>(std::move(dst_name), std::move(src));
    }

    if (tag == json_format::SET) {
        check_fields(body, here, {json_format::DST, json_format::IDX, json_format::VALUE});

        ExprPtr dst = expression(body.at(json_format::DST), field_path(here, json_format::DST));
        ExprPtr idx = expression(body.at(json_format::IDX), field_path(here, json_format::IDX));
        ExprPtr value = expression(body.at(json_format::VALUE), field_path(here, json_format::VALUE));

        return std::make_unique<AssignElemStmt>(std::move(dst), std::move(idx), std::move(value));
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

    if (tag == json_format::SWITCH) {
        check_fields(body, here, {json_format::SCRUTINEE, json_format::BRANCHES});

        ExprPtr scrutinee = expression(body.at(json_format::SCRUTINEE), field_path(here, json_format::SCRUTINEE));

        const Json& branches = body.at(json_format::BRANCHES);

        std::vector<std::pair<PatternPtr, StmtPtr>> cases;

        for (std::size_t i = 0; i < branches.size(); ++i) {
            std::string elem_path = path + json_format::PATH_SEPARATOR + 
                                    json_format::SWITCH + json_format::PATH_SEPARATOR +
                                    json_format::BRANCHES +"[" + std::to_string(i) + "]";

            cases.push_back(parse_case(branches[i], elem_path));
        }

        return std::make_unique<SwitchStmt>(std::move(scrutinee), std::move(cases));
    }

    if (tag == json_format::SCOPE) {
        check_object(body, here);
        std::vector<StmtPtr> decls;
        if (body.contains(json_format::DECLS)) {
            const Json& decls_json = body.at(json_format::DECLS);
            if (!decls_json.is_array()) {
                fail(field_path(here, json_format::DECLS), "expected array of declarations");
            }

            for (std::size_t i = 0; i < decls_json.size(); ++i) {
                std::string decl_path = field_path(here, json_format::DECLS) + "[" + std::to_string(i) + "]";
                decls.push_back(parse_decl(decls_json[i], decl_path));
            }
        }

        StmtPtr body_stmt;
        if (body.contains(json_format::BODY)) {
            const Json& body_json = body.at(json_format::BODY);
            if (body_json.is_array()) {
                if (body_json.empty()) {
                    body_stmt = std::make_unique<SkipStmt>();
                } else {
                    body_stmt = statement(body_json[0], field_path(here, json_format::BODY) + "[0]");
                    for (std::size_t i = 1; i < body_json.size(); ++i) {
                        StmtPtr next = statement(body_json[i], field_path(here, json_format::BODY) + "[" + std::to_string(i) + "]");
                        body_stmt = std::make_unique<SeqStmt>(std::move(body_stmt), std::move(next));
                    }
                }
            } else {
                body_stmt = statement(body_json, field_path(here, json_format::BODY));
            }
        } else {
            body_stmt = std::make_unique<SkipStmt>();
        }

        return std::make_unique<ScopeStmt>(std::move(decls), std::move(body_stmt));
    }

    if (tag == json_format::RETURN) {
        if (body.is_null()) {
            return std::make_unique<ReturnStmt>(nullptr);
        }

        if (body.is_object()) {
            for (const auto& key : {json_format::EXPR, json_format::VALUE}) {
                if (body.contains(key)) {
                    return std::make_unique<ReturnStmt>(expression(body.at(key), field_path(here, key)));
                }
            }
        }

        return std::make_unique<ReturnStmt>(expression(body, here));
    }

    if (tag == json_format::CALL) {
        check_object(body, here);
        std::string name;
        for (const auto& key : {json_format::FUN, json_format::NAME, json_format::VAR}) {
            if (body.contains(key)) {
                name = identifier(body.at(key), field_path(here, key));
                break;
            }
        }

        if (name.empty()) {
            fail(here, "missing function name in call");
        }

        std::vector<ExprPtr> args;
        std::string args_key = body.contains(json_format::ARGS) ? json_format::ARGS : json_format::PARAMS;
        if (body.contains(args_key)) {
            const Json& args_json = body.at(args_key);
            if (!args_json.is_array()) {
                fail(field_path(here, args_key), "expected array of arguments");
            }

            for (std::size_t i = 0; i < args_json.size(); ++i) {
                std::string arg_path = field_path(here, args_key) + "[" + std::to_string(i) + "]";
                args.push_back(expression(args_json[i], arg_path));
            }
        }

        return std::make_unique<CallStmt>(std::move(name), std::move(args));
    }

    if (tag == json_format::VAR) {
        return parse_var_decl(body, here);
    }

    if (tag == json_format::FUN) {
        return parse_fun_decl(body, here);
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
