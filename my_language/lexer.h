#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//#include <regex>
//#include <cctype>

#include "token.h"
#include "helper.h"

struct Lexer {
    std::string code;
    std::vector<Token> tokens;
    //std::regex patternInt{R"(^\d+$)"};
    //std::regex patternName{R"(^[a-zA-Z_][a-zA-Z0-9_]*$)"};

    int index;
    int currLine = 1;

    Lexer(std::string codeURL) : code(codeURL) {
        lex();
    }

    void lex();

    void scanNumeric(int currPosition, const std::string& line);
    void scanIdentifier(int currPosition, const std::string& line);
    void scanOperant(int currPosition, const std::string& line);

    void pushIdentifier(std::string identifier, int currPosition);
    void printTokens();
};

#endif