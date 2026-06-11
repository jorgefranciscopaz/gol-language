#ifndef PARSER_H
#define PARSER_H

#include "Lexer.hpp"
#include <vector>
#include <string>

// ─── Type ─────────────────────────────────────────────────────────────────────

enum class TypeKind { Int, Bool };

// ─── Expressions ──────────────────────────────────────────────────────────────

struct Expr
{
    virtual ~Expr() {}
};

struct IntLitExpr : Expr
{
    int value;
};

struct BoolLitExpr : Expr
{
    bool value;
};

struct IdentExpr : Expr
{
    std::string name;
};

struct Arg
{
    bool  byRef;
    Expr *value;
};

struct CallExpr : Expr
{
    std::string      name;
    std::vector<Arg> args;
};

struct UnaryExpr : Expr
{
    TokenId op;
    Expr   *operand;
};

struct BinaryExpr : Expr
{
    TokenId op;
    Expr   *left;
    Expr   *right;
};

// ─── Statements ───────────────────────────────────────────────────────────────

struct Stmt
{
    virtual ~Stmt() {}
};

struct Block
{
    std::vector<Stmt *> stmts;
};

struct VarDeclStmt : Stmt
{
    std::string name;
    TypeKind    type;
    Expr       *init; // nullptr if no initializer
};

struct ShortDeclStmt : Stmt
{
    std::string name;
    Expr       *value;
};

struct AssignStmt : Stmt
{
    std::string name;
    Expr       *value;
};

struct ElseIfClause
{
    Expr  *condition;
    Block  body;
};

struct IfStmt : Stmt
{
    Expr                     *condition;
    Block                     thenBlock;
    std::vector<ElseIfClause> elseIfs;
    Block                    *elseBlock; // nullptr if no else
};

struct ForStmt : Stmt
{
    Expr  *condition;
    Block  body;
};

struct ReturnStmt : Stmt
{
    Expr *value; // nullptr if bare return
};

struct CallStmt : Stmt
{
    std::string      name;
    std::vector<Arg> args;
};

struct PrintArg
{
    bool        isString;
    std::string strValue;  // used when isString == true
    Expr       *exprValue; // used when isString == false
};

struct PrintStmt : Stmt
{
    bool                  newline; // true = println, false = print
    std::vector<PrintArg> args;
};

// ─── Top-level declarations ───────────────────────────────────────────────────

struct Param
{
    bool        byRef;
    std::string name;
    TypeKind    type;
};

struct VarDecl
{
    std::string name;
    TypeKind    type;
    Expr       *init; // nullptr if no initializer
};

struct FuncDecl
{
    std::string        name;
    std::vector<Param> params;
    bool               hasReturnType;
    TypeKind           returnType;
    Block              body;
};

// ─── Program ──────────────────────────────────────────────────────────────────

struct Program
{
    std::vector<VarDecl *>  vars;
    std::vector<FuncDecl *> funcs;
};

// ─── Parser ───────────────────────────────────────────────────────────────────

class Parser
{
private:
    Lexer &lexer;
    Token  current;

    // Helpers
    void  advance();
    Token eat(TokenId id);
    bool  at(TokenId id);
    void  error(const std::string &msg);

    // Grammar productions — one method per rule
    TypeKind type();

    Expr *expr();
    Expr *orExpr();
    Expr *andExpr();
    Expr *notExpr();
    Expr *relExpr();
    Expr *addExpr();
    Expr *mulExpr();
    Expr *unaryExpr();
    Expr *primary();

    Arg              arg();
    std::vector<Arg> argList();

    PrintArg printArg();

    Block          block();
    Stmt          *stmt();
    VarDeclStmt   *varDeclStmt();
    ShortDeclStmt *shortDecl(const std::string &name);
    AssignStmt    *assignStmt(const std::string &name);
    IfStmt        *ifStmt();
    ForStmt       *forStmt();
    ReturnStmt    *returnStmt();
    CallStmt      *callStmt(const std::string &name);
    PrintStmt     *printStmt();

    Param              param();
    std::vector<Param> paramList();
    VarDecl           *varDecl();
    FuncDecl          *funcDecl();

public:
    Parser(Lexer &lexer_);
    Program *program();
};

#endif
