#pragma once

#include <string>

namespace json_format {
inline const std::string CONST = "const";
inline const std::string VAR = "var";
inline const std::string BINOP = "binop";
inline const std::string LEFT = "left";
inline const std::string RIGHT = "right";
inline const std::string READ = "read";
inline const std::string WRITE = "write";
inline const std::string SKIP = "skip";
inline const std::string SEQ = "seq";
inline const std::string ASSN = "assn";
inline const std::string WHILE = "while";
inline const std::string DO_WHILE = "do_while";
inline const std::string IF = "if";
inline const std::string DST = "dst";
inline const std::string SRC = "src";
inline const std::string COND = "cond";
inline const std::string BODY = "body";
inline const std::string INIT = "init";
inline const std::string STEP = "step";
inline const std::string THEN = "then";
inline const std::string ELSE = "else";

inline const std::string ROOT_PATH = "$";
inline const std::string PATH_SEPARATOR = ".";
}


namespace binop_text {
inline const std::string OR = "!!";
inline const std::string AND = "&&";
inline const std::string EQUAL = "==";
inline const std::string NOT_EQUAL = "!=";
inline const std::string LESS_EQUAL = "<=";
inline const std::string LESS = "<";
inline const std::string GREATER_EQUAL = ">=";
inline const std::string GREATER = ">";
inline const std::string ADD = "+";
inline const std::string SUBTRACT = "-";
inline const std::string MULTIPLY = "*";
inline const std::string DIVIDE = "/";
inline const std::string MODULO = "%";
}
