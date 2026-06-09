#ifndef LEXER_H
#define LEXER_H

#include <istream>
#include <string>

// ─── Token ID ─────────────────────────────────────────────────────────────────
enum class TokenId
{
    // Literals
    IntLit, BoolLit, StringLit,

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

    // Assignment
    OpAssign,    // =
    OpShortDecl, // :=

    // Address-of
    OpAmpersand, // &

    // Punctuation
    LParen, RParen, LBrace, RBrace, Comma, Semicolon,

    // Misc
    Ident, Eof, Unknown
};

// ─── Token ────────────────────────────────────────────────────────────────────
struct Token
{
    TokenId id;
    std::string text;
    int line{1};
    int column{1};

    std::string toString() const;
};

// ─── Lexer ────────────────────────────────────────────────────────────────────
class Lexer
{
private:
    std::istream &in;
    int currentChar;
    int line;
    int column;

    void advance();

public:
    Lexer(std::istream &in_) : in(in_), line(1), column(0)
    {
        currentChar = in.get();
        column++;
    }

    int getLine()   const { return line;   }
    int getColumn() const { return column; }

    Token nextToken();

    static const char *tokenToString(TokenId id);
};

#endif
