#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include <memory>

#include "token.h"
#include "helper.h"

struct Parser {
    struct NodeAST {
        Token token;
        std::unique_ptr<NodeAST> left = nullptr;
        std::unique_ptr<NodeAST> right = nullptr;
        std::vector<std::unique_ptr<NodeAST>> statements;
        std::unique_ptr<NodeAST> condition = nullptr;
        std::unique_ptr<NodeAST> increment = nullptr; // for loop
    
        NodeAST(Token newToken) :
            token(newToken) {}
        NodeAST(Token newToken, std::unique_ptr<NodeAST> newLeft) :
            token(newToken),
            left(std::move(newLeft)) {}
        NodeAST(Token newToken, std::unique_ptr<NodeAST> newLeft, std::unique_ptr<NodeAST> newRight) :
            token(newToken),
            left(std::move(newLeft)),
            right(std::move(newRight)) {}
        NodeAST(Token newToken, std::unique_ptr<NodeAST> newCondition, std::unique_ptr<NodeAST> newLeft, std::unique_ptr<NodeAST> newRight) :
            token(newToken),
            condition(std::move(newCondition)),
            left(std::move(newLeft)),
            right(std::move(newRight)) {}
        NodeAST(Token newToken, std::vector<std::unique_ptr<NodeAST>> newStatements) :
            token(newToken),
            statements(std::move(newStatements)) {}
        NodeAST(Token newToken, std::vector<std::unique_ptr<NodeAST>> newStatements, std::unique_ptr<NodeAST> newLeft) :
            token(newToken),
            statements(std::move(newStatements)),
            left(std::move(newLeft)) {}
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

    std::vector<Token> tokens;
    int index = 0;
    std::unique_ptr<NodeAST> ASTroot;

    Parser(const std::vector<Token>& newTokens) : tokens(newTokens) {
        createAST();
    }

    void createAST() {
        ASTroot = std::move(createBlock());
    }

    std::unique_ptr<NodeAST> createBlock();
    std::unique_ptr<NodeAST> createStatement();
    std::unique_ptr<NodeAST> createExpression();
    std::unique_ptr<NodeAST> parseAddSub();
    std::unique_ptr<NodeAST> parseMulDiv();
    std::unique_ptr<NodeAST> parseNeg();
    std::unique_ptr<NodeAST> parsePrimary();
    
    bool match(Token::Type type);
    bool matchNext(Token::Type type);

    std::unique_ptr<NodeAST> createIfNode();
    std::unique_ptr<NodeAST> createWhileNode();
    std::unique_ptr<NodeAST> createForNode();
    std::unique_ptr<NodeAST> createDeclarationNode();
    std::unique_ptr<NodeAST> createAssignmentNode(std::unique_ptr<NodeAST> left);
    void consumeSNI();
};

#endif