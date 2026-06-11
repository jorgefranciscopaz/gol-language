#include "Parser.hpp"
#include <stdexcept>

// ─── Helpers ──────────────────────────────────────────────────────────────────

void Parser::advance()
{
    current = lexer.nextToken();
}

// Consume the current token only if it matches id, otherwise throw.
Token Parser::eat(TokenId id)
{
    if (current.id != id)
    {
        error(std::string("expected '") + Lexer::tokenToString(id) +
              "' but got '" + Lexer::tokenToString(current.id) + "'");
    }

    Token tok = current;
    advance();
    return tok;
}

// Non-consuming peek: is the current token id?
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
    advance(); // prime the first token
}

// ─── program = { topDecl } EOF ────────────────────────────────────────────────

Program *Parser::program()
{
    Program *prog = new Program();

    while (!at(TokenId::Eof))
    {
        if (at(TokenId::KwVar))
            prog->vars.push_back(varDecl());
        else if (at(TokenId::KwFunc))
            prog->funcs.push_back(funcDecl());
        else
            error("expected 'var' or 'func' at top level");
    }

    eat(TokenId::Eof);
    return prog;
}

// ─── type = "int" | "bool" ────────────────────────────────────────────────────

TypeKind Parser::type()
{
    if (at(TokenId::KwInt))
    {
        eat(TokenId::KwInt);
        return TypeKind::Int;
    }

    if (at(TokenId::KwBool))
    {
        eat(TokenId::KwBool);
        return TypeKind::Bool;
    }

    error("expected 'int' or 'bool'");
    return TypeKind::Int; // unreachable
}

// ─── varDecl = "var" IDENTIFIER type [ "=" expr ] ";" ────────────────────────

VarDecl *Parser::varDecl()
{
    eat(TokenId::KwVar);
    Token name = eat(TokenId::Ident);

    VarDecl *node = new VarDecl();
    node->name = name.text;
    node->type = type();
    node->init = nullptr;

    if (at(TokenId::OpAssign))
    {
        eat(TokenId::OpAssign);
        node->init = expr();
    }

    eat(TokenId::Semicolon);
    return node;
}

// ─── funcDecl = "func" IDENTIFIER "(" [ paramList ] ")" [ type ] block ───────

FuncDecl *Parser::funcDecl()
{
    eat(TokenId::KwFunc);
    Token name = eat(TokenId::Ident);
    eat(TokenId::LParen);

    FuncDecl *node    = new FuncDecl();
    node->name        = name.text;
    node->hasReturnType = false;

    if (!at(TokenId::RParen))
        node->params = paramList();

    eat(TokenId::RParen);

    if (at(TokenId::KwInt) || at(TokenId::KwBool))
    {
        node->hasReturnType = true;
        node->returnType    = type();
    }

    node->body = block();
    return node;
}

// ─── paramList = param { "," param } ─────────────────────────────────────────

std::vector<Param> Parser::paramList()
{
    std::vector<Param> params;
    params.push_back(param());

    while (at(TokenId::Comma))
    {
        eat(TokenId::Comma);
        params.push_back(param());
    }

    return params;
}

// ─── param = [ "ref" ] IDENTIFIER type ───────────────────────────────────────

Param Parser::param()
{
    Param p;
    p.byRef = false;

    if (at(TokenId::KwRef))
    {
        eat(TokenId::KwRef);
        p.byRef = true;
    }

    Token name = eat(TokenId::Ident);
    p.name = name.text;
    p.type = type();

    return p;
}

// ─── block = "{" { stmt } "}" ────────────────────────────────────────────────

Block Parser::block()
{
    eat(TokenId::LBrace);

    Block b;
    while (!at(TokenId::RBrace))
        b.stmts.push_back(stmt());

    eat(TokenId::RBrace);
    return b;
}

// ─── stmt = varDecl | shortDecl | assignStmt | ifStmt | forStmt
//          | returnStmt | callStmt | printStmt ──────────────────────────────────

Stmt *Parser::stmt()
{
    if (at(TokenId::KwVar))
        return varDeclStmt();

    if (at(TokenId::KwIf))
        return ifStmt();

    if (at(TokenId::KwFor))
        return forStmt();

    if (at(TokenId::KwReturn))
        return returnStmt();

    if (at(TokenId::KwPrint) || at(TokenId::KwPrintln))
        return printStmt();

    // shortDecl | assignStmt | callStmt all start with IDENTIFIER
    if (at(TokenId::Ident))
    {
        Token name = eat(TokenId::Ident);

        if (at(TokenId::OpShortDecl))
            return shortDecl(name.text);

        if (at(TokenId::OpAssign))
            return assignStmt(name.text);

        if (at(TokenId::LParen))
            return callStmt(name.text);

        error("expected ':=', '=', or '(' after identifier");
    }

    error("unexpected token in statement");
    return nullptr;
}

// ─── varDeclStmt = "var" IDENTIFIER type [ "=" expr ] ";" ────────────────────

VarDeclStmt *Parser::varDeclStmt()
{
    eat(TokenId::KwVar);
    Token name = eat(TokenId::Ident);

    VarDeclStmt *node = new VarDeclStmt();
    node->name = name.text;
    node->type = type();
    node->init = nullptr;

    if (at(TokenId::OpAssign))
    {
        eat(TokenId::OpAssign);
        node->init = expr();
    }

    eat(TokenId::Semicolon);
    return node;
}

// ─── shortDecl = IDENTIFIER ":=" expr ";" ────────────────────────────────────

ShortDeclStmt *Parser::shortDecl(const std::string &name)
{
    eat(TokenId::OpShortDecl);

    ShortDeclStmt *node = new ShortDeclStmt();
    node->name  = name;
    node->value = expr();

    eat(TokenId::Semicolon);
    return node;
}

// ─── assignStmt = IDENTIFIER "=" expr ";" ────────────────────────────────────

AssignStmt *Parser::assignStmt(const std::string &name)
{
    eat(TokenId::OpAssign);

    AssignStmt *node = new AssignStmt();
    node->name  = name;
    node->value = expr();

    eat(TokenId::Semicolon);
    return node;
}

// ─── ifStmt = "if" expr block { "else" "if" expr block } [ "else" block ] ────

IfStmt *Parser::ifStmt()
{
    eat(TokenId::KwIf);

    IfStmt *node    = new IfStmt();
    node->condition = expr();
    node->thenBlock = block();
    node->elseBlock = nullptr;

    while (at(TokenId::KwElse))
    {
        eat(TokenId::KwElse);

        if (at(TokenId::KwIf))
        {
            eat(TokenId::KwIf);

            ElseIfClause clause;
            clause.condition = expr();
            clause.body      = block();
            node->elseIfs.push_back(clause);
        }
        else
        {
            node->elseBlock  = new Block();
            *node->elseBlock = block();
            break;
        }
    }

    return node;
}

// ─── forStmt = "for" expr block ──────────────────────────────────────────────

ForStmt *Parser::forStmt()
{
    eat(TokenId::KwFor);

    ForStmt *node   = new ForStmt();
    node->condition = expr();
    node->body      = block();

    return node;
}

// ─── returnStmt = "return" [ expr ] ";" ──────────────────────────────────────

ReturnStmt *Parser::returnStmt()
{
    eat(TokenId::KwReturn);

    ReturnStmt *node = new ReturnStmt();
    node->value      = nullptr;

    if (!at(TokenId::Semicolon))
        node->value = expr();

    eat(TokenId::Semicolon);
    return node;
}

// ─── callStmt = IDENTIFIER "(" [ argList ] ")" ";" ───────────────────────────

CallStmt *Parser::callStmt(const std::string &name)
{
    eat(TokenId::LParen);

    CallStmt *node = new CallStmt();
    node->name     = name;

    if (!at(TokenId::RParen))
        node->args = argList();

    eat(TokenId::RParen);
    eat(TokenId::Semicolon);
    return node;
}

// ─── printStmt = ("print"|"println") "(" printArg { "," printArg } ")" ";" ───

PrintStmt *Parser::printStmt()
{
    PrintStmt *node = new PrintStmt();

    if (at(TokenId::KwPrintln))
    {
        eat(TokenId::KwPrintln);
        node->newline = true;
    }
    else
    {
        eat(TokenId::KwPrint);
        node->newline = false;
    }

    eat(TokenId::LParen);
    node->args.push_back(printArg());

    while (at(TokenId::Comma))
    {
        eat(TokenId::Comma);
        node->args.push_back(printArg());
    }

    eat(TokenId::RParen);
    eat(TokenId::Semicolon);
    return node;
}

// ─── printArg = expr | STRING_LIT ────────────────────────────────────────────

PrintArg Parser::printArg()
{
    PrintArg pa;

    if (at(TokenId::StringLit))
    {
        pa.isString  = true;
        pa.strValue  = current.text;
        pa.exprValue = nullptr;
        eat(TokenId::StringLit);
    }
    else
    {
        pa.isString  = false;
        pa.exprValue = expr();
    }

    return pa;
}

// ─── argList = arg { "," arg } ───────────────────────────────────────────────

std::vector<Arg> Parser::argList()
{
    std::vector<Arg> args;
    args.push_back(arg());

    while (at(TokenId::Comma))
    {
        eat(TokenId::Comma);
        args.push_back(arg());
    }

    return args;
}

// ─── arg = [ "&" ] expr ──────────────────────────────────────────────────────

Arg Parser::arg()
{
    Arg a;
    a.byRef = false;

    if (at(TokenId::OpAmpersand))
    {
        eat(TokenId::OpAmpersand);
        a.byRef = true;
    }

    a.value = expr();
    return a;
}

// ─── expr = orExpr ───────────────────────────────────────────────────────────

Expr *Parser::expr()
{
    return orExpr();
}

// ─── orExpr = andExpr { "||" andExpr } ───────────────────────────────────────

Expr *Parser::orExpr()
{
    Expr *left = andExpr();

    while (at(TokenId::OpOr))
    {
        TokenId op = current.id;
        advance();

        BinaryExpr *node = new BinaryExpr();
        node->op    = op;
        node->left  = left;
        node->right = andExpr();
        left = node;
    }

    return left;
}

// ─── andExpr = notExpr { "&&" notExpr } ──────────────────────────────────────

Expr *Parser::andExpr()
{
    Expr *left = notExpr();

    while (at(TokenId::OpAnd))
    {
        TokenId op = current.id;
        advance();

        BinaryExpr *node = new BinaryExpr();
        node->op    = op;
        node->left  = left;
        node->right = notExpr();
        left = node;
    }

    return left;
}

// ─── notExpr = "!" notExpr | relExpr ─────────────────────────────────────────

Expr *Parser::notExpr()
{
    if (at(TokenId::OpNot))
    {
        TokenId op = current.id;
        advance();

        UnaryExpr *node = new UnaryExpr();
        node->op      = op;
        node->operand = notExpr();
        return node;
    }

    return relExpr();
}

// ─── relExpr = addExpr [ relOp addExpr ] ─────────────────────────────────────

Expr *Parser::relExpr()
{
    Expr *left = addExpr();

    if (at(TokenId::OpEq)  || at(TokenId::OpNeq) ||
        at(TokenId::OpLt)  || at(TokenId::OpGt)  ||
        at(TokenId::OpLe)  || at(TokenId::OpGe))
    {
        TokenId op = current.id;
        advance();

        BinaryExpr *node = new BinaryExpr();
        node->op    = op;
        node->left  = left;
        node->right = addExpr();
        return node;
    }

    return left;
}

// ─── addExpr = mulExpr { ("+" | "-") mulExpr } ───────────────────────────────

Expr *Parser::addExpr()
{
    Expr *left = mulExpr();

    while (at(TokenId::OpPlus) || at(TokenId::OpMinus))
    {
        TokenId op = current.id;
        advance();

        BinaryExpr *node = new BinaryExpr();
        node->op    = op;
        node->left  = left;
        node->right = mulExpr();
        left = node;
    }

    return left;
}

// ─── mulExpr = unaryExpr { ("*" | "/" | "%") unaryExpr } ─────────────────────

Expr *Parser::mulExpr()
{
    Expr *left = unaryExpr();

    while (at(TokenId::OpStar) || at(TokenId::OpSlash) || at(TokenId::OpPercent))
    {
        TokenId op = current.id;
        advance();

        BinaryExpr *node = new BinaryExpr();
        node->op    = op;
        node->left  = left;
        node->right = unaryExpr();
        left = node;
    }

    return left;
}

// ─── unaryExpr = "-" unaryExpr | primary ─────────────────────────────────────

Expr *Parser::unaryExpr()
{
    if (at(TokenId::OpMinus))
    {
        TokenId op = current.id;
        advance();

        UnaryExpr *node = new UnaryExpr();
        node->op      = op;
        node->operand = unaryExpr();
        return node;
    }

    return primary();
}

// ─── primary = INT_LIT | "true" | "false" | IDENTIFIER | callExpr | "(" expr ")"

Expr *Parser::primary()
{
    if (at(TokenId::IntLit))
    {
        Token tok = eat(TokenId::IntLit);

        IntLitExpr *node = new IntLitExpr();
        node->value      = std::stoi(tok.text);
        return node;
    }

    if (at(TokenId::KwTrue))
    {
        eat(TokenId::KwTrue);

        BoolLitExpr *node = new BoolLitExpr();
        node->value       = true;
        return node;
    }

    if (at(TokenId::KwFalse))
    {
        eat(TokenId::KwFalse);

        BoolLitExpr *node = new BoolLitExpr();
        node->value       = false;
        return node;
    }

    // IDENTIFIER or callExpr — both start with an identifier
    if (at(TokenId::Ident))
    {
        Token name = eat(TokenId::Ident);

        // callExpr = IDENTIFIER "(" [ argList ] ")"
        if (at(TokenId::LParen))
        {
            eat(TokenId::LParen);

            CallExpr *node = new CallExpr();
            node->name     = name.text;

            if (!at(TokenId::RParen))
                node->args = argList();

            eat(TokenId::RParen);
            return node;
        }

        IdentExpr *node = new IdentExpr();
        node->name      = name.text;
        return node;
    }

    if (at(TokenId::LParen))
    {
        eat(TokenId::LParen);
        Expr *inner = expr();
        eat(TokenId::RParen);
        return inner;
    }

    error("expected an expression");
    return nullptr;
}
