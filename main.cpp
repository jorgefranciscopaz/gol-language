#include "Lexer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[])
{
    // Leer de archivo si se pasa como argumento, si no leer de stdin
    std::ifstream file;
    std::istream *input = &std::cin;

    if (argc == 2)
    {
        file.open(argv[1]);

        if (!file.is_open())
        {
            std::cerr << "Error: no se pudo abrir el archivo '" << argv[1] << "'\n";
            return 1;
        }

        input = &file;
    }

    Lexer lexer(*input);

    while (true)
    {
        Token token = lexer.nextToken();

        std::cout << token.toString() << "\n";

        if (token.id == TokenId::Eof)
        {
            break;
        }
    }

    return 0;
}
