// -----------------------------------------------------------------------------
// Parser
// -----------------------------------------------------------------------------
// Converts a sequence of tokens into an Abstract Syntax Tree (AST).
// Uses recursive descent parsing.
// -----------------------------------------------------------------------------

#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include <memory>

#include "token.h"
#include "helper.h"

// -----------------------------------------------------------------------------
// Parser Structure
// -----------------------------------------------------------------------------
// Holds:
// - token stream from the lexer
// - current parsing index
// - root of the constructed AST
// -----------------------------------------------------------------------------
struct Parser {
    // -----------------------------------------------------------------------------
    // Abstract Syntax Tree Node Structure
    // -----------------------------------------------------------------------------
    // Represents a single AST node.
    // A node may represent:
    // - expressions (binary/unary via left/right)
    // - control flow (if/while/for via condition/statements)
    // - blocks (statements vector)
    // -----------------------------------------------------------------------------
    struct NodeAST {
        Token token;
        std::unique_ptr<NodeAST> left = nullptr;
        std::unique_ptr<NodeAST> right = nullptr;
        std::vector<std::unique_ptr<NodeAST>> statements; // used by if statement, loops
        std::unique_ptr<NodeAST> condition = nullptr; // used in loops
        std::unique_ptr<NodeAST> increment = nullptr; // for loop
    
        // -------------------------------------------------------------------------
        // Constructors
        // -------------------------------------------------------------------------
        
        // Leaf node (literals, variables, simple tokens)
        NodeAST(Token newToken) :
            token(newToken) {}

        // Unary node (e.g., Neg)
        NodeAST(Token newToken, std::unique_ptr<NodeAST> newLeft) :
            token(newToken),
            left(std::move(newLeft)) {}

        // Binary node (e.g., Add, Sub, Assign, comparisons)
        NodeAST(Token newToken, std::unique_ptr<NodeAST> newLeft, std::unique_ptr<NodeAST> newRight) :
            token(newToken),
            left(std::move(newLeft)),
            right(std::move(newRight)) {}

        // Conditional node (if / while)
        NodeAST(Token newToken, std::unique_ptr<NodeAST> newCondition, std::unique_ptr<NodeAST> newLeft, std::unique_ptr<NodeAST> newRight) :
            token(newToken),
            condition(std::move(newCondition)),
            left(std::move(newLeft)),
            right(std::move(newRight)) {}

        // Block or function call arguments container
        NodeAST(Token newToken, std::vector<std::unique_ptr<NodeAST>> newStatements) :
            token(newToken),
            statements(std::move(newStatements)) {}

        // Function definition
        NodeAST(Token newToken, std::vector<std::unique_ptr<NodeAST>> newStatements, std::unique_ptr<NodeAST> newLeft) :
            token(newToken),
            statements(std::move(newStatements)),
            left(std::move(newLeft)) {}
        
        // For loop
        NodeAST(
            Token newToken,
            std::unique_ptr<NodeAST> newCondition,
            std::unique_ptr<NodeAST> newLeft,
            std::unique_ptr<NodeAST> newRight,
            std::unique_ptr<NodeAST> newIncrement
        ) :
            token(newToken),
            condition(std::move(newCondition)),
            left(std::move(newLeft)),
            right(std::move(newRight)),
            increment(std::move(newIncrement)) {}
    };

    std::vector<Token> tokens; //reference to lexer's tokens
    int index = 0; // current token
    std::unique_ptr<NodeAST> ASTroot; // root to abstract syntax tree

    // -------------------------------------------------------------------------
    // Constructor
    // -------------------------------------------------------------------------
    Parser(const std::vector<Token>& newTokens) : tokens(newTokens) {
        createAST();
    }

    // -----------------------------------------------------------------------------
    // Start parsing
    // -----------------------------------------------------------------------------
    // Entry point for AST construction.
    // -----------------------------------------------------------------------------
    void createAST() {
        ASTroot = std::move(createBlock());
    }

    // -------------------------------------------------------------------------
    // Recursive descent functions
    // -------------------------------------------------------------------------
    std::unique_ptr<NodeAST> createBlock();
    std::unique_ptr<NodeAST> createStatement();
    std::unique_ptr<NodeAST> createExpression();
    std::unique_ptr<NodeAST> parseAddSub();
    std::unique_ptr<NodeAST> parseMulDiv();
    std::unique_ptr<NodeAST> parseNeg();
    std::unique_ptr<NodeAST> parsePrimary();
    
    // -------------------------------------------------------------------------
    // Matching functions
    // -------------------------------------------------------------------------
    bool match(Token::Type type);
    bool matchNext(Token::Type type);

    // -------------------------------------------------------------------------
    // Create nodes helping functions
    // -------------------------------------------------------------------------
    std::unique_ptr<NodeAST> createIfNode();
    std::unique_ptr<NodeAST> createWhileNode();
    std::unique_ptr<NodeAST> createForNode();
    std::unique_ptr<NodeAST> createDeclarationNode();
    std::unique_ptr<NodeAST> createAssignmentNode(std::unique_ptr<NodeAST> left);
    void consumeSNI(); // Check and consume Semicolon, New line and Indentation tokens
};

#endif