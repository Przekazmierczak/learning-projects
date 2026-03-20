// -----------------------------------------------------------------------------
// Token
// -----------------------------------------------------------------------------
// Defines the Token structure used to represent lexical tokens.
// -----------------------------------------------------------------------------

#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <string>

// -----------------------------------------------------------------------------
// Token Structure
// -----------------------------------------------------------------------------
// Represents a token with its type, value, and source position.
// -----------------------------------------------------------------------------
struct Token {
    enum class Type {
        Dec,
        Int, Bool, String,
        Add, Sub, Mul, Div,
        Lpar, Rpar,
        Var, Arg,
        NewL,
        Ind, Ded,
        Block,
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

    int val = 0; // Integer value
    bool bval = 0; // Boolean value
    std::string strval = ""; // Identifier name or string value
    
    int line = -1; // Line number in source
    int position = -1; // Position in line

    // -------------------------------------------------------------------------
    // Constructors
    // -------------------------------------------------------------------------

    // Used for tokens without additional data (e.g., Block, Call)
    Token(Type newType)
        : type(newType) {}

    // Used for tokens that only store position info (operators, keywords, etc.)
    Token(Type newType, int newLine, int newPosition)
        : type(newType), line(newLine), position(newPosition) {}

    // Used for integer
    Token(Type newType, int newVal, int newLine, int newPosition)
        : type(newType),  val(newVal), line(newLine), position(newPosition) {}

    // Used for string literals and identifiers (Var, Arg)
    Token(Type newType, std::string newStrval, int newLine, int newPosition)
        : type(newType),  strval(newStrval), line(newLine), position(newPosition) {}

    // Used for boolean
    Token(Type newType, bool newBval, int newLine, int newPosition)
        : type(newType),  bval(newBval), line(newLine), position(newPosition) {}
};


// -------------------------------------------------------------------------
// Token type names table
// -------------------------------------------------------------------------
// String representations of Token::Type values used for debugging output.
// -------------------------------------------------------------------------
static const char* names[] = {
    "Dec",
    "Int", "Bool", "String",
    "Add", "Sub", "Mul", "Div",
    "Lpar", "Rpar",
    "Var", "Arg",
    "NewL",
    "Ind", "Ded",
    "Block",
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

// -----------------------------------------------------------------------------
// Stream output operator
// -----------------------------------------------------------------------------
std::ostream& operator << (std::ostream& cout, const Token& token);

#endif