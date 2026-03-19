// -----------------------------------------------------------------------------
// Lexer
// -----------------------------------------------------------------------------
// Converts source code into a sequence of tokens.
// Supports numbers, identifiers, strings, operators, and indentation-based syntax.
//
// Tracks line numbers and indentation levels to emit appropriate tokens
// including Indent, Dedent, Newline, and various operators.
// -----------------------------------------------------------------------------

#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "token.h"
#include "helper.h"

// -----------------------------------------------------------------------------
// Lexer Structure
// -----------------------------------------------------------------------------
// Stores source file path, tokens, current index, and current line.
struct Lexer {
    std::string sourceCode; // Path to source code file
    std::vector<Token> tokens; // Tokens produced by the lexer
    int index; // Current character index in line
    int currLine = 1; // Current line number for error reporting

    // -------------------------------------------------------------------------
    // Constructor
    // -------------------------------------------------------------------------
    Lexer(std::string codeURL) : sourceCode(codeURL) {
        lex();
    }

    // -------------------------------------------------------------------------
    // Main Lexing Loop
    // -------------------------------------------------------------------------
    void lex();

    // -------------------------------------------------------------------------
    // Indentation Handling
    // -------------------------------------------------------------------------
    void checkCurrentIndentation();

    // -------------------------------------------------------------------------
    // Scanning Functions
    // -------------------------------------------------------------------------
    void scanNumeric(const std::string& line); // Scan numeric literal
    void scanIdentifier(const std::string& line);  // Scan identifier or keyword
    void scanString(const std::string& line); // Scan string literal
    void scanOperator(const std::string& line); // Scan operator

    // -------------------------------------------------------------------------
    // Helping Functions
    // -------------------------------------------------------------------------
    void addIdentifierToken(std::string identifier); // Convert identifier to keyword or variable
    void debugPrintTokens(); // Print all tokens (for debugging)
};

#endif