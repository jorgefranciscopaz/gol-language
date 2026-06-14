#include "Parser.hpp"
#include <stdexcept>

// ─── Helpers ──────────────────────────────────────────────────────────────────

void Parser::advance()
{
    current = lexer.nextToken();
}

Token Parser::eat(TokenId id)
{
    if (current.id != id)
        error(std::string("expected '") + Lexer::tokenToString(id) +
              "' but got '" + Lexer::tokenToString(current.id) + "'");

    Token tok = current;
    advance();
    return tok;
}

bool Parser::at(TokenId id)
{
    return current.id == id;
}

void Parser::error(const std::string &msg)
{
    throw std::runtime_error(
        msg + " at line " + std::to_string(current.line) +
        ", column " + std::to_string(current.column));
}

// ─── Constructor ──────────────────────────────────────────────────────────────

Parser::Parser(Lexer &lexer_) : lexer(lexer_)
{
    advance();
}

// ─── program = { topDecl } EOF ────────────────────────────────────────────────

void Parser::program()
{
    while (!at(TokenId::Eof))
    {
        if (at(TokenId::KwVar))
            varDecl();
        else if (at(TokenId::KwFunc))
            funcDecl();
        else
            error("expected 'var' or 'func' at top level");
    }

    eat(TokenId::Eof);
}

// ─── type = "int" | "bool" ────────────────────────────────────────────────────

void Parser::type()
{
    if (at(TokenId::KwInt))       { eat(TokenId::KwInt);  return; }
    if (at(TokenId::KwBool))      { eat(TokenId::KwBool); return; }

    error("expected 'int' or 'bool'");
}

// ─── varDecl = "var" IDENTIFIER type [ "=" expr ] ";" ────────────────────────

void Parser::varDecl()
{
    eat(TokenId::KwVar);
    eat(TokenId::Ident);
    type();

    if (at(TokenId::OpAssign))
    {
        eat(TokenId::OpAssign);
        expr();
    }

    eat(TokenId::Semicolon);
}

// ─── funcDecl = "func" IDENTIFIER "(" [ paramList ] ")" [ type ] block ───────

void Parser::funcDecl()
{
    eat(TokenId::KwFunc);
    eat(TokenId::Ident);
    eat(TokenId::LParen);

    if (!at(TokenId::RParen))
        paramList();

    eat(TokenId::RParen);

    if (at(TokenId::KwInt) || at(TokenId::KwBool))
        type();

    block();
}

// ─── paramList = param { "," param } ─────────────────────────────────────────

void Parser::paramList()
{
    param();

    while (at(TokenId::Comma))
    {
        eat(TokenId::Comma);
        param();
    }
}

// ─── param = [ "ref" ] IDENTIFIER type ───────────────────────────────────────

void Parser::param()
{
    if (at(TokenId::KwRef))
        eat(TokenId::KwRef);

    eat(TokenId::Ident);
    type();
}

// ─── block = "{" { stmt } "}" ────────────────────────────────────────────────

void Parser::block()
{
    eat(TokenId::LBrace);

    while (!at(TokenId::RBrace))
        stmt();

    eat(TokenId::RBrace);
}

// ─── stmt ─────────────────────────────────────────────────────────────────────

void Parser::stmt()
{
    if (at(TokenId::KwVar))    { varDeclStmt(); return; }
    if (at(TokenId::KwIf))     { ifStmt();      return; }
    if (at(TokenId::KwFor))    { forStmt();      return; }
    if (at(TokenId::KwReturn)) { returnStmt();   return; }

    if (at(TokenId::KwPrint) || at(TokenId::KwPrintln))
    {
        printStmt();
        return;
    }

    if (at(TokenId::Ident))
    {
        Token name = eat(TokenId::Ident);

        if (at(TokenId::OpShortDecl)) { shortDecl(name.text); return; }
        if (at(TokenId::OpAssign))    { assignStmt(name.text); return; }
        if (at(TokenId::LParen))      { callStmt(name.text);   return; }

        error("expected ':=', '=', or '(' after identifier");
    }

    error("unexpected token in statement");
}

// ─── varDeclStmt = "var" IDENTIFIER type [ "=" expr ] ";" ────────────────────

void Parser::varDeclStmt()
{
    eat(TokenId::KwVar);
    eat(TokenId::Ident);
    type();

    if (at(TokenId::OpAssign))
    {
        eat(TokenId::OpAssign);
        expr();
    }

    eat(TokenId::Semicolon);
}

// ─── shortDecl = IDENTIFIER ":=" expr ";" ────────────────────────────────────

void Parser::shortDecl(const std::string &)
{
    eat(TokenId::OpShortDecl);
    expr();
    eat(TokenId::Semicolon);
}

// ─── assignStmt = IDENTIFIER "=" expr ";" ────────────────────────────────────

void Parser::assignStmt(const std::string &)
{
    eat(TokenId::OpAssign);
    expr();
    eat(TokenId::Semicolon);
}

// ─── ifStmt = "if" expr block { "else" "if" expr block } [ "else" block ] ────

void Parser::ifStmt()
{
    eat(TokenId::KwIf);
    expr();
    block();

    while (at(TokenId::KwElse))
    {
        eat(TokenId::KwElse);

        if (at(TokenId::KwIf))
        {
            eat(TokenId::KwIf);
            expr();
            block();
        }
        else
        {
            block();
            break;
        }
    }
}

// ─── forStmt = "for" expr block ──────────────────────────────────────────────

void Parser::forStmt()
{
    eat(TokenId::KwFor);
    expr();
    block();
}

// ─── returnStmt = "return" [ expr ] ";" ──────────────────────────────────────

void Parser::returnStmt()
{
    eat(TokenId::KwReturn);

    if (!at(TokenId::Semicolon))
        expr();

    eat(TokenId::Semicolon);
}

// ─── callStmt = IDENTIFIER "(" [ argList ] ")" ";" ───────────────────────────

void Parser::callStmt(const std::string &)
{
    eat(TokenId::LParen);

    if (!at(TokenId::RParen))
        argList();

    eat(TokenId::RParen);
    eat(TokenId::Semicolon);
}

// ─── printStmt = ("print"|"println") "(" printArg { "," printArg } ")" ";" ───

void Parser::printStmt()
{
    if (at(TokenId::KwPrintln)) eat(TokenId::KwPrintln);
    else                        eat(TokenId::KwPrint);

    eat(TokenId::LParen);
    printArg();

    while (at(TokenId::Comma))
    {
        eat(TokenId::Comma);
        printArg();
    }

    eat(TokenId::RParen);
    eat(TokenId::Semicolon);
}

// ─── printArg = expr | STRING_LIT ────────────────────────────────────────────

void Parser::printArg()
{
    if (at(TokenId::StringLit))
        eat(TokenId::StringLit);
    else
        expr();
}

// ─── argList = arg { "," arg } ───────────────────────────────────────────────

void Parser::argList()
{
    arg();

    while (at(TokenId::Comma))
    {
        eat(TokenId::Comma);
        arg();
    }
}

// ─── arg = [ "&" ] expr ──────────────────────────────────────────────────────

void Parser::arg()
{
    if (at(TokenId::OpAmpersand))
        eat(TokenId::OpAmpersand);

    expr();
}

// ─── expr = orExpr ───────────────────────────────────────────────────────────

void Parser::expr()   { orExpr(); }

// ─── orExpr = andExpr { "||" andExpr } ───────────────────────────────────────

void Parser::orExpr()
{
    andExpr();

    while (at(TokenId::OpOr))
    {
        eat(TokenId::OpOr);
        andExpr();
    }
}

// ─── andExpr = notExpr { "&&" notExpr } ──────────────────────────────────────

void Parser::andExpr()
{
    notExpr();

    while (at(TokenId::OpAnd))
    {
        eat(TokenId::OpAnd);
        notExpr();
    }
}

// ─── notExpr = "!" notExpr | relExpr ─────────────────────────────────────────

void Parser::notExpr()
{
    if (at(TokenId::OpNot))
    {
        eat(TokenId::OpNot);
        notExpr();
        return;
    }

    relExpr();
}

// ─── relExpr = addExpr [ relOp addExpr ] ─────────────────────────────────────

void Parser::relExpr()
{
    addExpr();

    if (at(TokenId::OpEq)  || at(TokenId::OpNeq) ||
        at(TokenId::OpLt)  || at(TokenId::OpGt)  ||
        at(TokenId::OpLe)  || at(TokenId::OpGe))
    {
        advance();
        addExpr();
    }
}

// ─── addExpr = mulExpr { ("+" | "-") mulExpr } ───────────────────────────────

void Parser::addExpr()
{
    mulExpr();

    while (at(TokenId::OpPlus) || at(TokenId::OpMinus))
    {
        advance();
        mulExpr();
    }
}

// ─── mulExpr = unaryExpr { ("*" | "/" | "%") unaryExpr } ─────────────────────

void Parser::mulExpr()
{
    unaryExpr();

    while (at(TokenId::OpStar) || at(TokenId::OpSlash) || at(TokenId::OpPercent))
    {
        advance();
        unaryExpr();
    }
}

// ─── unaryExpr = "-" unaryExpr | primary ─────────────────────────────────────

void Parser::unaryExpr()
{
    if (at(TokenId::OpMinus))
    {
        eat(TokenId::OpMinus);
        unaryExpr();
        return;
    }

    primary();
}

// ─── primary = INT_LIT | "true" | "false" | IDENTIFIER | callExpr | "(" expr ")"

void Parser::primary()
{
    if (at(TokenId::IntLit))  { eat(TokenId::IntLit);  return; }
    if (at(TokenId::KwTrue))  { eat(TokenId::KwTrue);  return; }
    if (at(TokenId::KwFalse)) { eat(TokenId::KwFalse); return; }

    if (at(TokenId::Ident))
    {
        eat(TokenId::Ident);

        if (at(TokenId::LParen))          // callExpr
        {
            eat(TokenId::LParen);
            if (!at(TokenId::RParen))
                argList();
            eat(TokenId::RParen);
        }

        return;
    }

    if (at(TokenId::LParen))
    {
        eat(TokenId::LParen);
        expr();
        eat(TokenId::RParen);
        return;
    }

    error("expected an expression");
}
