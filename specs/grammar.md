# Grammar

## Table of Contents
- [Program Structure](#program-structure)
- [Declarations](#declarations)
- [Statements](#statements)
- [Function Arguments](#function-arguments)
- [Expressions](#expressions)
- [Primary Expressions](#primary-expressions)

---

## Program Structure

```
program     = { topDecl } EOF ;

topDecl     = varDecl
            | funcDecl ;
```

---

## Declarations

```
varDecl     = "var" IDENTIFIER type [ "=" expr ] ";" ;

type        = "int"
            | "bool" ;

funcDecl    = "func" IDENTIFIER "(" [ paramList ] ")"
              [ type ]
              block ;

paramList   = param { "," param } ;

param       = [ "ref" ] IDENTIFIER type ;
```

---

## Statements

```
block       = "{" { stmt } "}" ;

stmt        = varDecl
            | shortDecl
            | assignStmt
            | ifStmt
            | forStmt
            | returnStmt
            | callStmt
            | printStmt ;

shortDecl   = IDENTIFIER ":=" expr ";" ;

assignStmt  = IDENTIFIER "=" expr ";" ;

ifStmt      = "if" expr block
              { "else" "if" expr block }
              [ "else" block ] ;

forStmt     = "for" expr block ;

returnStmt  = "return" [ expr ] ";" ;

callStmt    = IDENTIFIER "(" [ argList ] ")" ";" ;

printStmt   = ("print" | "println")
              "(" printArg { "," printArg } ")"
              ";" ;

printArg    = expr
            | STRING_LIT ;
```

---

## Function Arguments

```
argList     = arg { "," arg } ;

arg         = [ "&" ] expr ;
```

---

## Expressions

```
expr        = orExpr ;

orExpr      = andExpr
              { "||" andExpr } ;

andExpr     = notExpr
              { "&&" notExpr } ;

notExpr     = "!" notExpr
            | relExpr ;

relExpr     = addExpr
              [ relOp addExpr ] ;

relOp       = "=="
            | "!="
            | "<"
            | ">"
            | "<="
            | ">=" ;

addExpr     = mulExpr
              { ("+" | "-") mulExpr } ;

mulExpr     = unaryExpr
              { ("*" | "/" | "%") unaryExpr } ;

unaryExpr   = "-" unaryExpr
            | primary ;
```

---

## Primary Expressions

```
primary     = INT_LIT
            | "true"
            | "false"
            | IDENTIFIER
            | callExpr
            | "(" expr ")" ;

callExpr    = IDENTIFIER
              "(" [ argList ] ")" ;
```