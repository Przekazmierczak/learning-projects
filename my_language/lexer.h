#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "token.h"
#include "helper.h"

struct Lexer {
    std::string sourceCode;
    std::vector<Token> tokens;

    int index;
    int currLine = 1;

    Lexer(std::string codeURL) : sourceCode(codeURL) {
        lex();
    }

    void lex();

    void checkCurrentIndentation();

    void scanNumeric(const std::string& line);
    void scanIdentifier(const std::string& line);
    void scanString(const std::string& line);
    void scanOperator(const std::string& line);

    void addIdentifierToken(std::string identifier);
    void debugPrintTokens();
};

#endif