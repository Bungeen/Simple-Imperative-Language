#include "lexer.h"
#include "compiler_error.h"
#include "operator.h"

#include <cctype>

constexpr std::string_view kDelimiters = "{}();=";

constexpr std::string_view kKeywords[] = {
    "read",
    "write",
    "while",
    "do",
    "for",
    "if",
    "else",
    "elif",
    "skip"
};

bool identifier_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '\'';
}

Lexer::Lexer(std::string_view source) 
    : source_(source)
{}

Token Lexer::next() {
    skip_ignored();

    Token token{Kind::End, {}, line_, column_};

    if (position_ == source_.size()) {
        return token;
    }

    const size_t begin = position_;
    const char c = source_[position_];

    if (std::islower(static_cast<unsigned char>(c))) {
        token.kind = Kind::Identifier;

        while (position_ < source_.size() && identifier_char(source_[position_])) {
            advance();
        }

        token.text = source_.substr(begin, position_ - begin);

        for (auto keyword : kKeywords) {
            if (token.text == keyword) {
                token.kind = Kind::Keyword;
                break;
            }
        }
        return token;
    }

    if (std::isdigit(static_cast<unsigned char>(c))) {
        token.kind = Kind::Number;

        while (position_ < source_.size() && std::isdigit(static_cast<unsigned char>(source_[position_]))) {
            advance();
        }

        token.text = source_.substr(begin, position_ - begin);
        return token;
    }

    token.kind = Kind::Symbol;
    for (const auto &op : kOperators) {
        if (starts(op.text) && op.text.size() > token.text.size()) {
            token.text = op.text;
        }
    }

    if (!token.text.empty()) {
        for (std::size_t i = 0; i < token.text.size(); ++i) {
            advance();
        }

        return token;
    }

    if (kDelimiters.find(c) != std::string_view::npos) {
        advance();
        token.text = source_.substr(begin, 1);
        return token;
    }

    fail(token, "Unexpected character '" + std::string(1, c) + "'");
}

void Lexer::skip_ignored() {
    while (position_ < source_.size()) {
        const char c = source_[position_];
        if (std::string_view(" \t\r\n\f\v").find(c) != std::string_view::npos) {
            advance();
            continue;
        }

        if (starts("--")) {
            while (position_ < source_.size() && source_[position_] != '\n') {
                advance();
            }
            continue;
        }

        if (starts("(*")) {
            const Token start{Kind::Symbol, "(*", line_, column_};
            advance();
            advance();

            while (position_ < source_.size() && !starts("*)")) {
                advance();
            }

            if (position_ == source_.size()) {
                fail(start, "Unterminated block comment");
            }

            advance();
            advance();

            continue;
        }

        break;
    }
}

bool Lexer::starts(std::string_view text) const {
    return source_.substr(position_, text.size()) == text;
}

void Lexer::advance() {
    if (source_[position_++] == '\n') {
        ++line_;
        column_ = 1;
        return;
    }

    ++column_;
}
