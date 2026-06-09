# Gol Lexer Token Specification

## Table of Contents
- [Ignored Input](#ignored-input)
- [Keywords](#keywords)
- [Identifiers](#identifiers)
- [Literals](#literals)
- [Arithmetic Operators](#arithmetic-operators)
- [Relational Operators](#relational-operators)
- [Boolean Operators](#boolean-operators)
- [Assignment Operators](#assignment-operators)
- [Reference Operator](#reference-operator)
- [Punctuation](#punctuation)
- [Special Tokens](#special-tokens)
- [Recommended Token Enum](#recommended-token-enum)

---

## Ignored Input

These elements are consumed by the lexer but do NOT produce tokens.

### Whitespace

```text
SPACE           ' '
TAB             '\t'
NEWLINE         '\n'
CARRIAGE_RETURN '\r'
```

### Single Line Comment

```text
// any text until newline
```

Example:

```gol
// this is a comment
```

### Multi Line Comment

```text
/* any text */
```

Example:

```gol
/*
   multi line
   comment
*/
```

---

## Keywords

| Token   | Lexeme  |
| ------- | ------- |
| VAR     | var     |
| FUNC    | func    |
| RETURN  | return  |
| IF      | if      |
| ELSE    | else    |
| FOR     | for     |
| TRUE    | true    |
| FALSE   | false   |
| INT     | int     |
| BOOL    | bool    |
| REF     | ref     |
| PRINT   | print   |
| PRINTLN | println |

---

## Identifiers

### IDENTIFIER

Pattern:

```regex
[a-zA-Z_][a-zA-Z0-9_]*
```

Examples:

```text
x
counter
_myVar
temp123
```

Notes:

* Case sensitive
* Keywords cannot be identifiers

---

## Literals

### Integer Literal

Token: `INT_LIT`

Pattern:

```regex
-?[0-9]+
```

Examples:

```text
0
42
-7
1000
```

### Boolean Literal

Token: `BOOL_LIT`

Lexemes:

```text
true
false
```

### String Literal

Token: `STRING_LIT`

Pattern:

```text
" any-char* "
```

Examples:

```text
"hello"
"world"
"hello\n"
```

Supported Escapes:

```text
\n
\t
\\
\"
```

---

## Arithmetic Operators

| Token   | Lexeme |
| ------- | ------ |
| PLUS    | +      |
| MINUS   | -      |
| STAR    | *      |
| SLASH   | /      |
| PERCENT | %      |

---

## Relational Operators

| Token | Lexeme |
| ----- | ------ |
| EQ    | ==     |
| NEQ   | !=     |
| LT    | <      |
| GT    | >      |
| LTE   | <=     |
| GTE   | >=     |

---

## Boolean Operators

| Token | Lexeme |
| ----- | ------ |
| AND   | &&     |
| OR    | \|\|   |
| NOT   | !      |

---

## Assignment Operators

| Token      | Lexeme |
| ---------- | ------ |
| ASSIGN     | =      |
| SHORT_DECL | :=     |

---

## Reference Operator

| Token     | Lexeme |
| --------- | ------ |
| AMPERSAND | &      |

Used for pass-by-reference calls.

Example:

```gol
increment(&x);
```

---

## Punctuation

| Token     | Lexeme |
| --------- | ------ |
| LPAREN    | (      |
| RPAREN    | )      |
| LBRACE    | {      |
| RBRACE    | }      |
| COMMA     | ,      |
| SEMICOLON | ;      |

---

## Special Tokens

### End Of File

Token: `ENDOFFILE`

Generated when the input stream is exhausted.

---

## Recommended Token Enum

```cpp
enum class TokenID
{
    // Special
    ENDOFFILE,
    ERROR,

    // Literals
    IDENTIFIER,
    INT_LIT,
    BOOL_LIT,
    STRING_LIT,

    // Keywords
    VAR,
    FUNC,
    RETURN,
    IF,
    ELSE,
    FOR,
    TRUE,
    FALSE,
    INT,
    BOOL,
    REF,
    PRINT,
    PRINTLN,

    // Arithmetic
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,

    // Relational
    EQ,
    NEQ,
    LT,
    GT,
    LTE,
    GTE,

    // Boolean
    AND,
    OR,
    NOT,

    // Assignment
    ASSIGN,
    SHORT_DECL,

    // Reference
    AMPERSAND,

    // Punctuation
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    COMMA,
    SEMICOLON
};
```
