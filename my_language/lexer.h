#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

#include "token.h"
#include "helper.h"

struct Lexer {
    std::string code;
    std::vector<Token> tokens;
    std::regex patternInt{R"(^\d+$)"};
    std::regex patternName{R"(^[a-zA-Z_][a-zA-Z0-9_]*$)"};

    Lexer(std::string codeURL) : code(codeURL) {
        lex();
    }

    void lex();
    void pushNonOperand(int currLine, int currPosition, std::string& current);
    void printTokens();
};

#endif