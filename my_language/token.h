#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <regex>

[[noreturn]] void throwError(std::string errorMsg) {
    std::cerr << errorMsg << std::endl;
    throw std::invalid_argument(errorMsg);
}

[[noreturn]] void throwError(std::string errorMsg, int line) {
    std::string newErrorMsg = errorMsg + "; Line: " + std::to_string(line);
    std::cerr << newErrorMsg << std::endl;
    throw std::invalid_argument(newErrorMsg);
}

[[noreturn]] void throwError(std::string errorMsg, int line, int position) {
    std::string newErrorMsg = errorMsg + "; Line: " + std::to_string(line) + ", Position: " + std::to_string(position);
    std::cerr << newErrorMsg << std::endl;
    throw std::invalid_argument(newErrorMsg);
}

struct Token {
    enum class Type {
        Int,
        Add, Sub, Mul, Div,
        Lpar, Rpar,
        Var,
        NewL,
        Ind, Ded,
        Block,
        Neg,
        Assign,
        If, Else,
        Scolon
    };

    Type type;

    int val = 0;
    std::string name = "";

    int line = -1;
    int position = -1;

    Token(Type newType)
        : type(newType) {}

    Token(Type newType, int newLine, int newPosition)
        : type(newType), line(newLine), position(newPosition) {}

    Token(Type newType, int newVal, int newLine, int newPosition)
        : type(newType),  val(newVal), line(newLine), position(newPosition) {}

    Token(Type newType, std::string newName, int newLine, int newPosition)
        : type(newType),  name(newName), line(newLine), position(newPosition) {}
};

static const char* names[] = {
    "Int",
    "Add", "Sub", "Mul", "Div",
    "Lpar", "Rpar",
    "Var",
    "NewL",
    "Ind", "Ded",
    "Block",
    "Neg",
    "Assign",
    "If", "Else",
    "Scolon",
};

std::ostream& operator << (std::ostream& cout, const Token& token)
{
    if (token.type == Token::Type::Int) {
        cout << "(" << names[static_cast<int>(token.type)] << ", " << token.val << ", [" << token.line << ", " << token.position << "])";
    } else if (token.type == Token::Type::Var) {
        cout << "(" << names[static_cast<int>(token.type)] << ", \"" << token.name << "\", [" << token.line << ", " << token.position << "])";
    } else {
        cout << "(" << names[static_cast<int>(token.type)] << ", [" << token.line << ", " << token.position << "])";
    }
    return cout;
}

#endif