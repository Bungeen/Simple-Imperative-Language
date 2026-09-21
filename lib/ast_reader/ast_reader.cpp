#include <string>
#include <memory>
#include <nlohmann/json.hpp>

#include "ast_reader.h"
#include "ast.h"

std::unique_ptr<IStmtNode> AstReader::ReadAst(const nlohmann::json& data) {
    return ReadAstStmt(data);
}

std::unique_ptr<IStmtNode> AstReader::ReadAstStmt(const nlohmann::json& data) {
    if (data.is_string() && data.get<std::string>() == "skip")
        return std::make_unique<SkipNode>();

    std::string key = data.begin().key();

    if (key == "seq") {
        std::unique_ptr<IStmtNode> lhs = ReadAstStmt(data[key].at("left"));
        std::unique_ptr<IStmtNode> rhs = ReadAstStmt(data[key].at("right"));

        return std::make_unique<SeqNode>(std::move(lhs), std::move(rhs));
    }

    if (key == "assn") {
        std::unique_ptr<ILvalueExprNode> dst = std::make_unique<VarNode>(data[key].at("dst"));
        std::unique_ptr<IExprNode> src = ReadAstExpr(data[key].at("src"));

        return std::make_unique<AssignNode>(std::move(dst), std::move(src));
    }

    if (key == "read") {
        std::unique_ptr<ILvalueExprNode> dst = std::make_unique<VarNode>(data[key]);

        return std::make_unique<ReadNode>(std::move(dst));
    }

    if (key == "write") {
        std::unique_ptr<IExprNode> text = ReadAstExpr(data[key]);

        return std::make_unique<WriteNode>(std::move(text));
    }

    if (key == "while") {
        std::unique_ptr<IStmtNode> body = ReadAstStmt(data[key].at("body"));
        std::unique_ptr<IExprNode> condition = ReadAstExpr(data[key].at("cond"));

        return std::make_unique<WhileNode>(std::move(condition), std::move(body));
    }

    if (key == "do") {
        std::unique_ptr<IStmtNode> body = ReadAstStmt(data[key].at("body"));
        std::unique_ptr<IExprNode> condition = ReadAstExpr(data[key].at("cond"));

        return std::make_unique<DoWhileNode>(std::move(condition), std::move(body));
    }

    if (key == "if") {
        std::unique_ptr<IExprNode> condition = ReadAstExpr(data[key].at("cond"));
        std::unique_ptr<IStmtNode> if_version = ReadAstStmt(data[key].at("then"));
        std::unique_ptr<IStmtNode> else_version = ReadAstStmt(data[key].at("else"));

        return std::make_unique<IfNode>(std::move(condition), std::move(if_version), std::move(else_version));
    }

    throw std::invalid_argument("Didn't manage to parse json ast");
}

std::unique_ptr<IExprNode> AstReader::ReadAstExpr(const nlohmann::json& data) {
    std::string key = data.begin().key();

    if (key == "const")
        return std::make_unique<ConstNode>(data[key]);

    if (key == "var") {
        return std::make_unique<VarNode>(data[key]);
    }

    if (key == "binop") {
        std::unique_ptr<IExprNode> lhs = ReadAstExpr(data.at("left"));
        std::unique_ptr<IExprNode> rhs = ReadAstExpr(data.at("right"));
        BinOp operation = operation_map.at(data.at(key));

        return std::make_unique<BinOpNode>(std::move(lhs), std::move(rhs), operation);
    }

    throw std::invalid_argument("Didn't manage to parse json ast");
}