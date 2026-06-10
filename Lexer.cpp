#include "Lexer.hpp"
#include <stdexcept>
#include <cctype>

// ─── States ───────────────────────────────────────────────────────────────────
enum class State
{
    Q0,
    NumberQ1,
    IdentQ1,
    StringQ1,
    StringEscape,
    SpacesQ1,
    LineCommentQ1,
    BlockCommentQ1,
    BlockCommentStar,
};

// ─── Keyword table ────────────────────────────────────────────────────────────
static TokenId lookupKeyword(const std::string &s)
{
    if (s == "var")     return TokenId::KwVar;
    if (s == "func")    return TokenId::KwFunc;
    if (s == "return")  return TokenId::KwReturn;
    if (s == "if")      return TokenId::KwIf;
    if (s == "else")    return TokenId::KwElse;
    if (s == "for")     return TokenId::KwFor;
    if (s == "true")    return TokenId::KwTrue;
    if (s == "false")   return TokenId::KwFalse;
    if (s == "int")     return TokenId::KwInt;
    if (s == "bool")    return TokenId::KwBool;
    if (s == "ref")     return TokenId::KwRef;
    if (s == "print")   return TokenId::KwPrint;
    if (s == "println") return TokenId::KwPrintln;

    return TokenId::Ident;
}

// ─── advance ──────────────────────────────────────────────────────────────────
void Lexer::advance()
{
    currentChar = in.get();

    if (currentChar == '\n')
    {
        line++;
        column = 0;
    }
    else
    {
        column++;
    }
}

// ─── nextToken ────────────────────────────────────────────────────────────────
Token Lexer::nextToken()
{
    State state = State::Q0;
    Token token;

    while (true)
    {
        switch (state)
        {

        // ── Initial state ──────────────────────────────────────────────────
        case State::Q0:

            token.text.clear();
            token.line   = line;
            token.column = column;

            if (currentChar == EOF)
            {
                token.id = TokenId::Eof;
                return token;
            }

            if (isspace(currentChar))
            {
                advance();
                state = State::SpacesQ1;
                break;
            }

            if (currentChar == '/')
            {
                int next = in.peek();

                if (next == '/')
                {
                    advance();
                    advance();
                    state = State::LineCommentQ1;
                    break;
                }

                if (next == '*')
                {
                    advance();
                    advance();
                    state = State::BlockCommentQ1;
                    break;
                }

                token.text = "/";
                advance();
                token.id = TokenId::OpSlash;
                return token;
            }

            if (currentChar == '"')
            {
                advance();
                state = State::StringQ1;
                break;
            }

            if (currentChar == '+')
            {
                token.text = "+";
                advance();
                token.id = TokenId::OpPlus;
                return token;
            }

            if (currentChar == '-')
            {
                token.text = "-";
                advance();
                token.id = TokenId::OpMinus;
                return token;
            }

            if (currentChar == '*')
            {
                token.text = "*";
                advance();
                token.id = TokenId::OpStar;
                return token;
            }

            if (currentChar == '%')
            {
                token.text = "%";
                advance();
                token.id = TokenId::OpPercent;
                return token;
            }

            if (currentChar == '=')
            {
                if (in.peek() == '=')
                {
                    token.text = "==";
                    advance();
                    advance();
                    token.id = TokenId::OpEq;
                    return token;
                }

                token.text = "=";
                advance();
                token.id = TokenId::OpAssign;
                return token;
            }

            if (currentChar == '!')
            {
                if (in.peek() == '=')
                {
                    token.text = "!=";
                    advance();
                    advance();
                    token.id = TokenId::OpNeq;
                    return token;
                }

                token.text = "!";
                advance();
                token.id = TokenId::OpNot;
                return token;
            }

            if (currentChar == '<')
            {
                if (in.peek() == '=')
                {
                    token.text = "<=";
                    advance();
                    advance();
                    token.id = TokenId::OpLe;
                    return token;
                }

                token.text = "<";
                advance();
                token.id = TokenId::OpLt;
                return token;
            }

            if (currentChar == '>')
            {
                if (in.peek() == '=')
                {
                    token.text = ">=";
                    advance();
                    advance();
                    token.id = TokenId::OpGe;
                    return token;
                }

                token.text = ">";
                advance();
                token.id = TokenId::OpGt;
                return token;
            }

            if (currentChar == '&')
            {
                if (in.peek() == '&')
                {
                    token.text = "&&";
                    advance();
                    advance();
                    token.id = TokenId::OpAnd;
                    return token;
                }

                token.text = "&";
                advance();
                token.id = TokenId::OpAmpersand;
                return token;
            }

            if (currentChar == '|')
            {
                if (in.peek() == '|')
                {
                    token.text = "||";
                    advance();
                    advance();
                    token.id = TokenId::OpOr;
                    return token;
                }

                throw std::runtime_error(
                    "Invalid token '|' at line " + std::to_string(line) +
                    ", column " + std::to_string(column));
            }

            if (currentChar == ':')
            {
                if (in.peek() == '=')
                {
                    token.text = ":=";
                    advance();
                    advance();
                    token.id = TokenId::OpShortDecl;
                    return token;
                }

                throw std::runtime_error(
                    "Invalid token ':' at line " + std::to_string(line) +
                    ", column " + std::to_string(column));
            }

            if (currentChar == ';')
            {
                token.text = ";";
                advance();
                token.id = TokenId::Semicolon;
                return token;
            }

            if (currentChar == ',')
            {
                token.text = ",";
                advance();
                token.id = TokenId::Comma;
                return token;
            }

            if (currentChar == '(')
            {
                token.text = "(";
                advance();
                token.id = TokenId::LParen;
                return token;
            }

            if (currentChar == ')')
            {
                token.text = ")";
                advance();
                token.id = TokenId::RParen;
                return token;
            }

            if (currentChar == '{')
            {
                token.text = "{";
                advance();
                token.id = TokenId::LBrace;
                return token;
            }

            if (currentChar == '}')
            {
                token.text = "}";
                advance();
                token.id = TokenId::RBrace;
                return token;
            }

            if (isdigit(currentChar))
            {
                token.text += currentChar;
                advance();
                state = State::NumberQ1;
                break;
            }

            if (isalpha(currentChar) || currentChar == '_')
            {
                token.text += currentChar;
                advance();
                state = State::IdentQ1;
                break;
            }

            throw std::runtime_error(
                std::string("Invalid character '") + char(currentChar) +
                "' at line " + std::to_string(line) +
                ", column " + std::to_string(column));

        // ── Skip whitespace ────────────────────────────────────────────────
        case State::SpacesQ1:
            if (isspace(currentChar))
            {
                advance();
                break;
            }
            state = State::Q0;
            break;

        // ── Line comment: skip until newline ───────────────────────────────
        case State::LineCommentQ1:
            if (currentChar != '\n' && currentChar != EOF)
            {
                advance();
                break;
            }
            state = State::Q0;
            break;

        // ── Block comment: skip until closing */ ───────────────────────────
        case State::BlockCommentQ1:
            if (currentChar == EOF)
            {
                throw std::runtime_error(
                    "Unterminated block comment starting at line " +
                    std::to_string(token.line));
            }

            if (currentChar == '*')
            {
                advance();
                state = State::BlockCommentStar;
                break;
            }

            advance();
            break;

        case State::BlockCommentStar:
            if (currentChar == '/')
            {
                advance();
                state = State::Q0;
                break;
            }

            if (currentChar == '*')
            {
                advance();
                break;
            }

            advance();
            state = State::BlockCommentQ1;
            break;

        // ── Integer literal ────────────────────────────────────────────────
        case State::NumberQ1:
            if (isdigit(currentChar))
            {
                token.text += currentChar;
                advance();
                break;
            }

            try
            {
                std::stoll(token.text);
            }
            catch (const std::out_of_range &)
            {
                throw std::runtime_error(
                    "Integer out of range at line " + std::to_string(token.line) +
                    ", column " + std::to_string(token.column));
            }

            token.id = TokenId::IntLit;
            return token;

        // ── String literal ─────────────────────────────────────────────────
        case State::StringQ1:
            if (currentChar == EOF || currentChar == '\n')
            {
                throw std::runtime_error(
                    "Unterminated string at line " + std::to_string(token.line) +
                    ", column " + std::to_string(token.column));
            }

            if (currentChar == '\\')
            {
                advance();
                state = State::StringEscape;
                break;
            }

            if (currentChar == '"')
            {
                advance();
                token.id = TokenId::StringLit;
                return token;
            }

            token.text += currentChar;
            advance();
            break;

        case State::StringEscape:
            if (currentChar == 'n')
            {
                token.text += '\n';
            }
            else if (currentChar == 't')
            {
                token.text += '\t';
            }
            else if (currentChar == '\\')
            {
                token.text += '\\';
            }
            else if (currentChar == '"')
            {
                token.text += '"';
            }
            else
            {
                throw std::runtime_error(
                    std::string("Invalid escape sequence '\\") + char(currentChar) +
                    "' at line " + std::to_string(line) +
                    ", column " + std::to_string(column));
            }

            advance();
            state = State::StringQ1;
            break;

        // ── Identifier / keyword ───────────────────────────────────────────
        case State::IdentQ1:
            if (isalnum(currentChar) || currentChar == '_')
            {
                token.text += currentChar;
                advance();
                break;
            }

            token.id = lookupKeyword(token.text);
            return token;

        default:
            throw std::runtime_error("Invalid lexer state");
        }
    }
}

// ─── tokenToString ────────────────────────────────────────────────────────────
const char *Lexer::tokenToString(TokenId id)
{
    switch (id)
    {
    case TokenId::IntLit:      return "INT_LIT";
    case TokenId::BoolLit:     return "BOOL_LIT";
    case TokenId::StringLit:   return "STRING_LIT";

    case TokenId::KwVar:       return "KW_VAR";
    case TokenId::KwFunc:      return "KW_FUNC";
    case TokenId::KwReturn:    return "KW_RETURN";
    case TokenId::KwIf:        return "KW_IF";
    case TokenId::KwElse:      return "KW_ELSE";
    case TokenId::KwFor:       return "KW_FOR";
    case TokenId::KwTrue:      return "KW_TRUE";
    case TokenId::KwFalse:     return "KW_FALSE";
    case TokenId::KwInt:       return "KW_INT";
    case TokenId::KwBool:      return "KW_BOOL";
    case TokenId::KwRef:       return "KW_REF";
    case TokenId::KwPrint:     return "KW_PRINT";
    case TokenId::KwPrintln:   return "KW_PRINTLN";

    case TokenId::OpPlus:      return "OP_PLUS";
    case TokenId::OpMinus:     return "OP_MINUS";
    case TokenId::OpStar:      return "OP_STAR";
    case TokenId::OpSlash:     return "OP_SLASH";
    case TokenId::OpPercent:   return "OP_PERCENT";

    case TokenId::OpEq:        return "OP_EQ";
    case TokenId::OpNeq:       return "OP_NEQ";
    case TokenId::OpLt:        return "OP_LT";
    case TokenId::OpGt:        return "OP_GT";
    case TokenId::OpLe:        return "OP_LE";
    case TokenId::OpGe:        return "OP_GE";

    case TokenId::OpAnd:       return "OP_AND";
    case TokenId::OpOr:        return "OP_OR";
    case TokenId::OpNot:       return "OP_NOT";

    case TokenId::OpAssign:    return "OP_ASSIGN";
    case TokenId::OpShortDecl: return "OP_SHORT_DECL";
    case TokenId::OpAmpersand: return "OP_AMPERSAND";

    case TokenId::LParen:      return "LPAREN";
    case TokenId::RParen:      return "RPAREN";
    case TokenId::LBrace:      return "LBRACE";
    case TokenId::RBrace:      return "RBRACE";
    case TokenId::Comma:       return "COMMA";
    case TokenId::Semicolon:   return "SEMICOLON";

    case TokenId::Ident:       return "IDENT";
    case TokenId::Eof:         return "EOF";
    case TokenId::Unknown:     return "UNKNOWN";
    }

    return "UNKNOWN";
}

// ─── Token::toString ──────────────────────────────────────────────────────────
std::string Token::toString() const
{
    std::string result = Lexer::tokenToString(id);
    result += "(\"";
    result += text;
    result += "\") at ";
    result += std::to_string(line);
    result += ":";
    result += std::to_string(column);
    return result;
}
