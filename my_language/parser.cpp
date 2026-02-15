#include "parser.h"

std::unique_ptr<Parser::NodeAST> Parser::createBlock() {
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

std::unique_ptr<Parser::NodeAST> Parser::createStatement() {
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

std::unique_ptr<Parser::NodeAST> Parser::createExpression() {
    std::unique_ptr<Parser::NodeAST> left = parseAddSub();
    while (
        index < tokens.size() &&
        (
            tokens[index].type == Token::Type::CmpEq ||
            tokens[index].type == Token::Type::CmpNEq  ||
            tokens[index].type == Token::Type::CmpGt ||
            tokens[index].type == Token::Type::CmpLs ||
            tokens[index].type == Token::Type::CmpGtEq ||
            tokens[index].type == Token::Type::CmpLsEq
        )
    ) {
        Token currToken = tokens[index];
        index++;
        std::unique_ptr<NodeAST> right = parseAddSub();
        left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
    }
    return left;
}

std::unique_ptr<Parser::NodeAST> Parser::parseAddSub() {
    std::unique_ptr<NodeAST> left = parseMulDiv();
    while (index < tokens.size() && (tokens[index].type == Token::Type::Add || tokens[index].type == Token::Type::Sub)) {
        Token currToken = tokens[index];
        index++;
        std::unique_ptr<NodeAST> right = parseMulDiv();
        left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
    }
    return left;
}

std::unique_ptr<Parser::NodeAST> Parser::parseMulDiv() {
    std::unique_ptr<NodeAST> left = parseNeg();
    while (index < tokens.size() && (tokens[index].type == Token::Type::Mul || tokens[index].type == Token::Type::Div)) {
        Token currToken = tokens[index];
        index++;
        std::unique_ptr<NodeAST> right = parseNeg();
        left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
    }
    return left;
}

std::unique_ptr<Parser::NodeAST> Parser::parseNeg() {
    if (index < tokens.size() && tokens[index].type == Token::Type::Sub) {
        Token negToken = Token(Token::Type::Neg, tokens[index].line, tokens[index].position);
        index++;
        std::unique_ptr<NodeAST> left = parseNeg();
        left = std::make_unique<NodeAST>(NodeAST(negToken, std::move(left)));
        return left;
    }
    return parsePrimary();
}

std::unique_ptr<Parser::NodeAST> Parser::parsePrimary() {
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

std::unique_ptr<Parser::NodeAST> Parser::createIfStatement() {
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
void Parser::consumeSNI() {
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