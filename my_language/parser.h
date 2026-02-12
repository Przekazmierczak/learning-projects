#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <regex>

#include "token.h"

struct Parser {
    struct NodeAST {
        Token token;
        std::unique_ptr<NodeAST> left = nullptr;
        std::unique_ptr<NodeAST> right = nullptr;
        std::vector<std::unique_ptr<NodeAST>> statements;
        std::unique_ptr<NodeAST> condition = nullptr;
    
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

    std::unique_ptr<NodeAST> createBlock() {
        Token newBlock = Token(Token::Type::Block);
        auto newBlockAST = std::make_unique<NodeAST>(newBlock);

        while (index < tokens.size()) {
            while (index < tokens.size() && tokens[index].type == Token::Type::NewL) {
                index++;
            }

            if (index >= tokens.size()) break;

            if (tokens[index].type == Token::Type::Ded) {
                index++; // consume Ded
                break;
            } else {
                newBlockAST->statements.push_back(std::move(createStatement()));
            }
        }

        return std::move(newBlockAST);
    }

    std::unique_ptr<NodeAST> createStatement() {
        if (index < tokens.size() && tokens[index].type == Token::Type::If) {
            return createIfStatement();
        }

        std::unique_ptr<NodeAST> left = createExpression();
        if (index < tokens.size() && tokens[index].type == Token::Type::Assign) {
            if (left->token.type == Token::Type::Var) {
                Token currToken = tokens[index];
                index++;
                std::unique_ptr<NodeAST> right = createExpression();
                left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
            } else {
                throwError("Incorrect left value", tokens[index].line);
            }
        } else if (index < tokens.size() && tokens[index].type == Token::Type::NewL) {
    
        } else {
            throwError("Incorrect symbol after expression", tokens[index].line, tokens[index].position);
        }
        return left;
    }

    std::unique_ptr<NodeAST> createExpression() {
        return parseAddSub();
    }
    
    std::unique_ptr<NodeAST> parseAddSub() {
        std::unique_ptr<NodeAST> left = parseMulDiv();
        while (index < tokens.size() && (tokens[index].type == Token::Type::Add || tokens[index].type == Token::Type::Sub)) {
            Token currToken = tokens[index];
            index++;
            std::unique_ptr<NodeAST> right = parseMulDiv();
            left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
        }
        return left;
    }

    std::unique_ptr<NodeAST> parseMulDiv() {
        std::unique_ptr<NodeAST> left = parseNeg();
        while (index < tokens.size() && (tokens[index].type == Token::Type::Mul || tokens[index].type == Token::Type::Div)) {
            Token currToken = tokens[index];
            index++;
            std::unique_ptr<NodeAST> right = parseNeg();
            left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
        }
        return left;
    }

    std::unique_ptr<NodeAST> parseNeg() {
        if (index < tokens.size() && tokens[index].type == Token::Type::Sub) {
            Token negToken = Token(Token::Type::Neg, tokens[index].line, tokens[index].position);
            index++;
            std::unique_ptr<NodeAST> left = parseNeg();
            left = std::make_unique<NodeAST>(NodeAST(negToken, std::move(left)));
            return left;
        }
        return parsePrimary();
    }

    std::unique_ptr<NodeAST> parsePrimary() {
        if (index < tokens.size() && (tokens[index].type == Token::Type::Int || tokens[index].type == Token::Type::Var)) {
            auto node = std::make_unique<NodeAST>(NodeAST(tokens[index]));
            index++;
            return node;
        }

        if (index < tokens.size() && tokens[index].type == Token::Type::Lpar) {
            index++; // consume "("
            std::unique_ptr<NodeAST> expression = createExpression();
            if (index >= tokens.size() || tokens[index].type != Token::Type::Rpar) {
                throwError("Right parenthesis is missing", tokens[index].line, tokens[index].position);
            }
            index++; // consume ")"
            return expression;
        }
        throwError("Syntax error", tokens[index].line, tokens[index].position);
    }

    std::unique_ptr<NodeAST> createIfStatement() {
        Token ifToken = tokens[index];
        index++; // consume if
        std::unique_ptr<NodeAST> condition = createExpression();
        consumeSNI();
        std::unique_ptr<NodeAST> ifBlock = createBlock();

        std::unique_ptr<NodeAST> elseBlock = nullptr;
        if (index < tokens.size() && tokens[index].type == Token::Type::Else) {
            index++; // consume else
            consumeSNI();
            elseBlock = createBlock();
        }

        return std::make_unique<NodeAST>(NodeAST(ifToken, std::move(condition), std::move(ifBlock), std::move(elseBlock)));
    }

    // Check and consume Semicolon, New line and Indentation tokens
    void consumeSNI() {
        if (index >= tokens.size() || tokens[index].type != Token::Type::Scolon) {
            throwError("\";\" is missing after if statemant", tokens[index].line, tokens[index].position);
        }
        index++; // consume ";"

        if (index >= tokens.size() || tokens[index].type != Token::Type::NewL) {
            throwError("New line is missing after if statemant", tokens[index].line, tokens[index].position);
        }
        index++; // consume NewL

        if (index >= tokens.size() || tokens[index].type != Token::Type::Ind) {
            throwError("Indentation is missing after if statemant", tokens[index].line, tokens[index].position);
        }
        index++; // consume Ind
    }
};

#endif