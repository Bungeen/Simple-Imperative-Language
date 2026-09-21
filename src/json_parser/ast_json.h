#pragma once

#include "ast.h"

#include <iosfwd>
#include <string>

namespace ast_json {

ExprPtr expr_from_json(const std::string& text);
StmtPtr stmt_from_json(const std::string& text);
StmtPtr read_program(std::istream& input);

}
