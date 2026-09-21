#include "interpreter.h"

#include <limits>
#include <stdexcept>


constexpr std::int64_t minimum = std::numeric_limits<std::int64_t>::min();
constexpr std::int64_t maximum = std::numeric_limits<std::int64_t>::max();

std::int64_t add(std::int64_t left, std::int64_t right) {
    if ((right > 0 && left > maximum - right)
        || (right < 0 && left < minimum - right)) {
        throw std::runtime_error("integer overflow in +");
    }

    return left + right;
}

std::int64_t subtract(std::int64_t left, std::int64_t right) {
    if ((right > 0 && left < minimum + right)
        || (right < 0 && left > maximum + right)) {
        throw std::runtime_error("integer overflow in -");
    }

    return left - right;
}

std::int64_t multiply(std::int64_t left, std::int64_t right) {
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

std::int64_t Interpreter::get_variable(const std::string& name) const {
    auto variable = variables.find(name);

    if (variable == variables.end()) {
        throw std::runtime_error("undefined variable: " + name);
    }

    return variable->second;
}

std::int64_t Interpreter::apply(BinOp op, std::int64_t left, const Expr& right) {
    if (op == BinOp::Or && left != 0) {
        return 1;
    }

    if (op == BinOp::And && left == 0) {
        return 0;
    }

    std::int64_t rhs = evaluate(right);

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
        return node->value;
    }

    if (const auto* node = dynamic_cast<const Var*>(&expression)) {
        return get_variable(node->name);
    }

    if (const auto* node = dynamic_cast<const BinOpExpr*>(&expression)) {
        std::int64_t left = evaluate(*node->lhs);

        return apply(node->op, left, *node->rhs);
    }

    throw std::runtime_error("unknown expression node");
}

void Interpreter::execute(const Stmt& statement) {
    if (const auto* node = dynamic_cast<const SeqStmt*>(&statement)) {
        execute(*node->lhs);
        execute(*node->rhs);

        return;
    }

    if (dynamic_cast<const SkipStmt*>(&statement)) {
        return;
    }

    if (const auto* node = dynamic_cast<const ReadStmt*>(&statement)) {
        std::string token;

        if (!(input >> token)) {
            throw std::runtime_error("read(" + node->name + "): expected an integer");
        }

        std::size_t consumed = 0;
        std::int64_t value;

        try {
            value = std::stoll(token, &consumed, 10);
        } catch (const std::exception&) {
            throw std::runtime_error("read(" + node->name + "): expected an int64 integer");
        }

        if (consumed != token.size()) {
            throw std::runtime_error("read(" + node->name + "): invalid integer: " + token);
        }

        variables[node->name] = value;

        return;
    }

    if (const auto* node = dynamic_cast<const WriteStmt*>(&statement)) {
        std::int64_t value = evaluate(*node->value);
        output << value << '\n';

        if (!output) {
            throw std::runtime_error("write: output failed");
        }

        return;
    }

    if (const auto* node = dynamic_cast<const AssignStmt*>(&statement)) {
        std::int64_t value = evaluate(*node->src);
        variables[node->dst] = value;

        return;
    }

    if (const auto* node = dynamic_cast<const CompoundAssignStmt*>(&statement)) {
        std::int64_t left = get_variable(node->dst);
        std::int64_t value = apply(node->op, left, *node->src);
        variables[node->dst] = value;

        return;
    }

    if (const auto* node = dynamic_cast<const IfStmt*>(&statement)) {
        if (evaluate(*node->cond) != 0) {
            execute(*node->thenBranch);
        } else if (node->elseBranch) {
            execute(*node->elseBranch);
        }

        return;
    }

    if (const auto* node = dynamic_cast<const WhileStmt*>(&statement)) {
        while (evaluate(*node->cond) != 0) {
            execute(*node->body);
        }

        return;
    }

    if (const auto* node = dynamic_cast<const DoWhileStmt*>(&statement)) {
        do {
            execute(*node->body);
        } while (evaluate(*node->cond) != 0);

        return;
    }

    if (const auto* node = dynamic_cast<const ForStmt*>(&statement)) {
        execute(*node->init);

        while (evaluate(*node->cond) != 0) {
            execute(*node->body);
            execute(*node->step);
        }

        return;
    }

    throw std::runtime_error("unknown statement node");
}
