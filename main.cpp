#include "Lexer.hpp"
#include "Parser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

static std::string readAll(std::istream &in)
{
    std::ostringstream buf;
    buf << in.rdbuf();
    return buf.str();
}

int main(int argc, char *argv[])
{
    std::ifstream file;
    std::istream *input = &std::cin;

    if (argc == 2)
    {
        file.open(argv[1]);
        if (!file.is_open())
        {
            std::cerr << "Error: no se pudo abrir '" << argv[1] << "'\n";
            return 1;
        }
        input = &file;
    }

    std::string src = readAll(*input);

    // ── Lexer output ──────────────────────────────────────────────────────────
    std::cout << "=== Lexer ===\n";
    {
        std::istringstream ss(src);
        Lexer lexer(ss);

        while (true)
        {
            Token tok = lexer.nextToken();
            std::cout << tok.toString() << "\n";
            if (tok.id == TokenId::Eof)
                break;
        }
    }

    // ── Parser output ─────────────────────────────────────────────────────────
    std::cout << "\n=== Parser ===\n";
    try
    {
        std::istringstream ss(src);
        Lexer  lexer(ss);
        Parser parser(lexer);

        parser.program();

        std::cout << "Parse OK\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error de sintaxis: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
