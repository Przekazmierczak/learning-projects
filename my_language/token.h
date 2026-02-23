#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <string>

struct Token {
    enum class Type {
        Dec,
        Int, Bool,
        Add, Sub, Mul, Div,
        Lpar, Rpar,
        Var, Arg,
        NewL,
        Ind, Ded,
        Block, FBlock,
        Neg,
        Assign,
        If, Else,
        Scolon,
        CmpEq, CmpNEq, CmpGt, CmpLs, CmpGtEq, CmpLsEq,
        While, For,
        And, Or,
        Fun, Call,
        Ret,
        Comma
    };

    Type type;

    int val = 0;
    std::string name = "";
    bool bval = 0;

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

    Token(Type newType, bool newBval, int newLine, int newPosition)
        : type(newType),  bval(newBval), line(newLine), position(newPosition) {}
};

static const char* names[] = {
    "Dec",
    "Int", "Bool",
    "Add", "Sub", "Mul", "Div",
    "Lpar", "Rpar",
    "Var", "Arg",
    "NewL",
    "Ind", "Ded",
    "Block", "FBlock",
    "Neg",
    "Assign",
    "If", "Else",
    "Scolon",
    "CmpEq", "CmpNEq", "CmpGt", "CmpLs", "CmpGtEq", "CmpLsEq",
    "While", "For",
    "And", "Or",
    "Fun", "Call",
    "Ret",
    "Comma"
};

std::ostream& operator << (std::ostream& cout, const Token& token);

#endif