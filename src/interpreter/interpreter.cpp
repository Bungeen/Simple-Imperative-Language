#include "interpreter.h"

#include <limits>
#include <stdexcept>


constexpr Value minimum = std::numeric_limits<Value>::min();
constexpr Value maximum = std::numeric_limits<Value>::max();

Value add(Value left, Value right) {
    if ((right > 0 && left > maximum - right)
        || (right < 0 && left < minimum - right)) {
        throw std::runtime_error("integer overflow in +");
    }

    return left + right;
}

Value subtract(Value left, Value right) {
    if ((right > 0 && left < minimum + right)
        || (right < 0 && left > maximum + right)) {
        throw std::runtime_error("integer overflow in -");
    }

    return left - right;
}

Value multiply(Value left, Value right) {
    bool overflow = false;

    if (left > 0) {
        if (right > 0) {
            overflow = left > maximum / right;
        } else if (right < 0) {
            overflow = right < minimum / left;
        }
    } else if (left < 0) {
        if (right > 0) {
            overflow = left < minimum / right;
        } else if (right < 0) {
            overflow = left < maximum / right;
        }
    }

    if (overflow) {
        throw std::runtime_error("integer overflow in *");
    }

    return left * right;
}


Interpreter::Interpreter(std::istream& input, std::ostream& output)
    : input(input)
    , output(output)
{}

Value Interpreter::get_variable(const std::string& name) const {
    auto variable = variables.find(name);

    if (variable == variables.end()) {
        throw std::runtime_error("undefined variable: " + name);
    }

    return variable->second;
}

Value Interpreter::apply(BinOp op, Value left, const Expr& right) {
    if (op == BinOp::Or && left != 0) {
        return 1;
    }

    if (op == BinOp::And && left == 0) {
        return 0;
    }

    Value rhs = evaluate(right);

    switch (op) {
        case BinOp::Or:
        case BinOp::And:
            return rhs != 0;

        case BinOp::Equal:
            return left == rhs;

        case BinOp::NotEqual:
            return left != rhs;

        case BinOp::LessEqual:
            return left <= rhs;

        case BinOp::Less:
            return left < rhs;

        case BinOp::GreaterEqual:
            return left >= rhs;

        case BinOp::Greater:
            return left > rhs;

        case BinOp::Add:
            return add(left, rhs);

        case BinOp::Subtract:
            return subtract(left, rhs);

        case BinOp::Multiply:
            return multiply(left, rhs);

        case BinOp::Divide:
            if (rhs == 0) {
                throw std::runtime_error("division by zero");
            }

            if (left == minimum && rhs == -1) {
                throw std::runtime_error("integer overflow in /");
            }

            return left / rhs;

        case BinOp::Modulo:
            if (rhs == 0) {
                throw std::runtime_error("modulo by zero");
            }

            if (left == minimum && rhs == -1) {
                return 0;
            }

            return left % rhs;
    }

    throw std::runtime_error("unknown binary operator");
}

std::int64_t Interpreter::evaluate(const Expr& expression) {
    if (const auto* node = dynamic_cast<const Const*>(&expression)) {
        return node->get_value();
    }

    if (const auto* node = dynamic_cast<const Var*>(&expression)) {
        return get_variable(node->get_var_name());
    }

    if (const auto* node = dynamic_cast<const BinOpExpr*>(&expression)) {
        std::int64_t left = evaluate(node->get_lhs());

        return apply(node->get_operation(), left, node->get_rhs());
    }

    throw std::runtime_error("unknown expression node");
}

void Interpreter::execute(const Stmt& statement) {
    if (const auto* node = dynamic_cast<const SeqStmt*>(&statement)) {
        execute(node->get_lhs());
        execute(node->get_rhs());

        return;
    }

    if (dynamic_cast<const SkipStmt*>(&statement)) {
        return;
    }

    if (const auto* node = dynamic_cast<const ReadStmt*>(&statement)) {
        std::string token;

        if (!(input >> token)) {
            throw std::runtime_error("read(" + node->get_variable_name() + "): expected an integer");
        }

        std::size_t consumed = 0;
        Value value;

        try {
            value = std::stoll(token, &consumed, 10);
        } catch (const std::exception&) {
            throw std::runtime_error("read(" + node->get_variable_name() + "): expected an int64 integer");
        }

        if (consumed != token.size()) {
            throw std::runtime_error("read(" + node->get_variable_name() + "): invalid integer: " + token);
        }

        variables[node->get_variable_name()] = value;

        return;
    }

    if (const auto* node = dynamic_cast<const WriteStmt*>(&statement)) {
        Value value = evaluate(node->get_value());
        output << value << '\n';

        if (!output) {
            throw std::runtime_error("write: output failed");
        }

        return;
    }

    if (const auto* node = dynamic_cast<const AssignStmt*>(&statement)) {
        Value value = evaluate(node->get_src());
        variables[node->get_dst()] = value;

        return;
    }

    if (const auto* node = dynamic_cast<const IfStmt*>(&statement)) {
        if (evaluate(node->get_condition()) != 0) {
            execute(node->get_then_branch());
        } else {
            execute(node->get_else_branch());
        }

        return;
    }

    if (const auto* node = dynamic_cast<const WhileStmt*>(&statement)) {
        while (evaluate(node->get_condition()) != 0) {
            execute(node->get_body());
        }

        return;
    }

    if (const auto* node = dynamic_cast<const DoWhileStmt*>(&statement)) {
        do {
            execute(node->get_body());
        } while (evaluate(node->get_condition()) != 0);

        return;
    }

    throw std::runtime_error("unknown statement node");
}
