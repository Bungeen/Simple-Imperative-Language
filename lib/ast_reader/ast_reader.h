#include <nlohmann/json.hpp>
#include <string>
#include <memory>

#include "ast.h"

class AstReader {
public:
    std::unique_ptr<IStmtNode> ReadAst(const nlohmann::json& data);
private:
    std::unique_ptr<IStmtNode> ReadAstStmt(const nlohmann::json& data);
    std::unique_ptr<IExprNode> ReadAstExpr(const nlohmann::json& data);

    const std::unordered_map<std::string, BinOp> operation_map = {
        {"+", BinOp::sum}, 
        {"-", BinOp::sub}, 
        {"*", BinOp::mult},
        {"/", BinOp::div}, 
        {"%", BinOp::remainder}, 
        {"==", BinOp::equal},
        {"!=", BinOp::nequal},
        {"<", BinOp::less},
        {">", BinOp::greater},
        {"<=", BinOp::less_eq},
        {">=", BinOp::greater_eq},
        {"&&", BinOp::logical_and},
        {"!!", BinOp::logical_or}
    };
};