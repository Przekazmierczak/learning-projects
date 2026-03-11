#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "token.h"
#include "helper.h"

struct Lexer {
    std::string code;
    std::vector<Token> tokens;

    int index;
    int currLine = 1;

    Lexer(std::string codeURL) : code(codeURL) {
        lex();
    }

    void lex();

    void checkCurrentIndentation();

    void scanNumeric(const std::string& line);
    void scanIdentifier(const std::string& line);
    void scanString(const std::string& line);
    void scanOperant(const std::string& line);

    void pushIdentifier(std::string identifier);
    void printTokens();
};

#endif