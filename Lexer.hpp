#pragma once
#include <istream>
#include <string>

// ─── Token ────────────────────────────────────────────────────────────────────
enum class TokenId {
    // Literals
    IntLit, StringLit,
    // Keywords
    KwVar, KwFunc, KwReturn,
    KwIf, KwElse, KwFor,
    KwTrue, KwFalse,
    KwInt, KwBool,
    KwRef, KwPrint, KwPrintln,
    // Arithmetic operators
    OpPlus, OpMinus, OpStar, OpSlash, OpPercent,
    // Relational operators
    OpEq, OpNeq, OpLt, OpGt, OpLe, OpGe,
    // Boolean operators
    OpAnd, OpOr, OpNot,
    // Assignment / short-decl
    OpAssign,     // =
    OpShortDecl,  // :=
    // Address-of
    OpAmpersand,  // &
    // Punctuation
    LParen, RParen, LBrace, RBrace, Comma, Semicolon,
    // Misc
    Ident, Eof, Unknown
};

struct Token {
    TokenId id;
    std::string text;
    int line{1};
    std::string toString() const;
};