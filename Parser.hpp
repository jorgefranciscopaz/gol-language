#ifndef PARSER_H
#define PARSER_H

#include "Lexer.hpp"
#include <string>

class Parser
{
private:
    Lexer &lexer;
    Token  current;

    void  advance();
    Token eat(TokenId id);
    bool  at(TokenId id);
    void  error(const std::string &msg);

    void type();

    void expr();
    void orExpr();
    void andExpr();
    void notExpr();
    void relExpr();
    void addExpr();
    void mulExpr();
    void unaryExpr();
    void primary();

    void arg();
    void argList();
    void printArg();

    void block();
    void stmt();
    void varDeclStmt();
    void shortDecl(const std::string &name);
    void assignStmt(const std::string &name);
    void ifStmt();
    void forStmt();
    void returnStmt();
    void callStmt(const std::string &name);
    void printStmt();

    void param();
    void paramList();
    void varDecl();
    void funcDecl();

public:
    Parser(Lexer &lexer_);
    void program();
};

#endif
