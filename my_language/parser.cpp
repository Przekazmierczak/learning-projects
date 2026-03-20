#include "parser.h"

// -----------------------------------------------------------------------------
// Create Block
// -----------------------------------------------------------------------------
// Parses a block of statements until Ded (dedentation) or end of input.
// Skips leading newlines.
// Returns: unique pointer to abstract syntax tree node.
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::createBlock() {
    Token newBlock = Token(Token::Type::Block);
    auto newBlockAST = std::make_unique<NodeAST>(newBlock);

    while (index < tokens.size()) {
        while (match(Token::Type::NewL)) index++;

        if (index >= tokens.size()) break;

        if (match(Token::Type::Ded)) {
            index++; // consume Ded
            break;
        }

        newBlockAST->statements.push_back(createStatement());
    }

    return std::move(newBlockAST);
}

// -----------------------------------------------------------------------------
// Create Statement
// -----------------------------------------------------------------------------
// Parses a single statement:
// - control flow (if, while, for)
// - function definition
// - declaration
// - return
// - expression / assignment
// Returns: unique pointer to abstract syntax tree node.
// Throws:
// - syntax errors for invalid constructs.
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::createStatement() {
    if (match(Token::Type::If)) {
        return createIfNode();
    }

    if (match(Token::Type::While)) {
        return createWhileNode();
    }
    
    if (match(Token::Type::For)) {
        return createForNode();
    }

    if (match(Token::Type::Dec)) {
        return createDeclarationNode();
    }

    if (match(Token::Type::Fun)) {
        Token funToken = tokens[index];
        index++;
        if (match(Token::Type::Var)) {
            funToken.strval = tokens[index].strval;
        } else {
            throwError("Incorrect syntax for function declaration", tokens[index].line);
        }
        index++;

        std::vector<std::unique_ptr<NodeAST>> argumentsAST;

        while (match(Token::Type::Var)) {
            tokens[index].type = Token::Type::Arg;
            argumentsAST.push_back(parsePrimary());

            if (match(Token::Type::Comma)) {
                index++;
            } else if (match(Token::Type::Scolon)){
                break;
            } else {
                throwError("Incorrect syntax for function declaration", tokens[index].line);
            }
        }

        consumeSNI();

        std::unique_ptr<NodeAST> funBlock = createBlock();

        return std::make_unique<NodeAST>(NodeAST(funToken, std::move(argumentsAST), std::move(funBlock)));
    }

    if (match(Token::Type::Ret)) {
        Token retToken = tokens[index];
        index++;
        return std::make_unique<NodeAST>(retToken, std::move(createExpression()));
    }

    std::unique_ptr<NodeAST> left = createExpression();
    
    if (match(Token::Type::Assign)) {
        return createAssignmentNode(std::move(left));
    }

    if (match(Token::Type::NewL)) {
        return left;
    }

    throwError("Incorrect symbol after expression", tokens[index].line, tokens[index].position);
}

// -----------------------------------------------------------------------------
// Create Expression
// -----------------------------------------------------------------------------
// Parses logical and comparison expressions.
// Lowest precedence level (after arithmetic).
// Returns: unique pointer to abstract syntax tree node.
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::createExpression() {
    std::unique_ptr<Parser::NodeAST> left = parseAddSub();
    while (
        match(Token::Type::CmpEq) ||
        match(Token::Type::CmpNEq)  ||
        match(Token::Type::CmpGt) ||
        match(Token::Type::CmpLs) ||
        match(Token::Type::CmpGtEq) ||
        match(Token::Type::CmpLsEq) ||
        match(Token::Type::And) ||
        match(Token::Type::Or)
    ) {
        Token currToken = tokens[index];
        index++;
        std::unique_ptr<NodeAST> right = parseAddSub();
        left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
    }
    return left;
}

// -----------------------------------------------------------------------------
// Parse Add/Sub
// -----------------------------------------------------------------------------
// Handles addition and subtraction.
// Returns: unique pointer to abstract syntax tree node
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::parseAddSub() {
    std::unique_ptr<NodeAST> left = parseMulDiv();
    while (match(Token::Type::Add) || match(Token::Type::Sub)) {
        Token currToken = tokens[index];
        index++;
        std::unique_ptr<NodeAST> right = parseMulDiv();
        left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
    }
    return left;
}

// -----------------------------------------------------------------------------
// Parse Mul/Div
// -----------------------------------------------------------------------------
// Handles multiplication and division (higher precedence than Add/Sub).
// Returns: unique pointer to abstract syntax tree node.
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::parseMulDiv() {
    std::unique_ptr<NodeAST> left = parseNeg();
    while (match(Token::Type::Mul) || match(Token::Type::Div)) {
        Token currToken = tokens[index];
        index++;
        std::unique_ptr<NodeAST> right = parseNeg();
        left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
    }
    return left;
}

// -----------------------------------------------------------------------------
// Parse Unary Negation
// -----------------------------------------------------------------------------
// Handles unary minus (right-associative).
// Returns: unique pointer to abstract syntax tree node.
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::parseNeg() {
    if (match(Token::Type::Sub)) {
        Token negToken = Token(Token::Type::Neg, tokens[index].line, tokens[index].position);
        index++;
        std::unique_ptr<NodeAST> left = parseNeg();
        return std::make_unique<NodeAST>(NodeAST(negToken, std::move(left)));
    }
    return parsePrimary();
}

// -----------------------------------------------------------------------------
// Parse Primary
// -----------------------------------------------------------------------------
// Parses:
// - literals (int, bool, string)
// - variables / arguments
// - function calls
// - parenthesized expressions
// Throws: syntax error if no valid primary expression is found.
// Returns: unique pointer to abstract syntax tree node.
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::parsePrimary() {
    if (match(Token::Type::Var) && matchNext(Token::Type::Lpar)) {
        Token callToken = tokens[index];
        callToken.type = Token::Type::Call;
        index++; // consume call
        index++; // consume lpar

        std::vector<std::unique_ptr<NodeAST>> argumentsAST;

        while (!match(Token::Type::Rpar)) {
            argumentsAST.push_back(createExpression());

            if (match(Token::Type::Comma)) {
                index++;
            } else if (match(Token::Type::Rpar)) {
                break;
            } else {
                throwError("Incorrect function call", tokens[index].line, tokens[index].position);
            }
        }

        index++; // consume rpar

        return std::make_unique<NodeAST>(NodeAST(callToken, std::move(argumentsAST)));
    }

    if (
        match(Token::Type::Int) ||
        match(Token::Type::Bool) ||
        match(Token::Type::String) ||
        match(Token::Type::Var) ||
        match(Token::Type::Arg)
    ) {
        auto node = std::make_unique<NodeAST>(NodeAST(tokens[index]));
        index++;
        return node;
    }

    if (match(Token::Type::Lpar)) {
        index++; // consume "("
        std::unique_ptr<NodeAST> expression = createExpression();
        if (index >= tokens.size() || tokens[index].type != Token::Type::Rpar) {
            throwError("Right parenthesis is missing", tokens[index].line, tokens[index].position);
        }
        index++; // consume ")"
        return expression;
    }
    throwError("Syntax error (incorrect primary)", tokens[index].line, tokens[index].position);
}

// -----------------------------------------------------------------------------
// Match Token
// -----------------------------------------------------------------------------
// Checks whether the current token matches the given type.
// Parameters:
//   type - token type
// Returns: true if current token match token from argument otherwise false.
// -----------------------------------------------------------------------------
bool Parser::match(Token::Type type) {
    if (index < tokens.size() && tokens[index].type == type) {
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
// Match Next Token
// -----------------------------------------------------------------------------
// Checks whether the next token matches the given type.
// Parameters:
//   type - token type
// Returns: true if next token match token from argument otherwise false.
// -----------------------------------------------------------------------------
bool Parser::matchNext(Token::Type type) {
    if (index + 1< tokens.size() && tokens[index + 1].type == type) {
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
// Create If Node
// -----------------------------------------------------------------------------
// Parses an if / else-if / else structure.
// condition -> condition expression
// left      -> if block
// right     -> else block or nested if (else-if)
// Returns: unique pointer to abstract syntax tree node.
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::createIfNode() {
    Token ifToken = tokens[index];
    index++; // consume if
    std::unique_ptr<NodeAST> condition = createExpression();
    consumeSNI();
    std::unique_ptr<NodeAST> ifBlock = createBlock();

    std::unique_ptr<NodeAST> elseBlock = nullptr;
    if (match(Token::Type::Else)) {
        index++; // consume else

        if (match(Token::Type::If)) {
            elseBlock = createIfNode();
        } else {
            consumeSNI();
            elseBlock = createBlock();
        }
    }

    return std::make_unique<NodeAST>(NodeAST(ifToken, std::move(condition), std::move(ifBlock), std::move(elseBlock)));
}

// -----------------------------------------------------------------------------
// Create While Node
// -----------------------------------------------------------------------------
// Parses a while loop.
// condition -> loop condition
// left      -> loop body
// Returns: unique pointer to abstract syntax tree node.
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::createWhileNode() {
    Token whileToken = tokens[index];
    index++; // consume while
    std::unique_ptr<NodeAST> condition = createExpression();
    consumeSNI();
    std::unique_ptr<NodeAST> whileBlock = createBlock();

    return std::make_unique<NodeAST>(NodeAST(whileToken, std::move(condition), std::move(whileBlock), nullptr));
}

// -----------------------------------------------------------------------------
// Create For Node
// -----------------------------------------------------------------------------
// Parses a for loop.
// right     -> initialization
// condition -> loop condition
// increment -> increment expression
// left      -> loop body
// Returns: unique pointer to abstract syntax tree node.
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::createForNode() {
    Token forToken = tokens[index];
    index++; // consume for
    std::unique_ptr<NodeAST> initial = nullptr;
    if (!match(Token::Type::Scolon)) {
        initial = createStatement();
    }

    std::unique_ptr<NodeAST> condition = nullptr;
    if (match(Token::Type::Scolon)) {
        index++; // consume semicolon
        if (!match(Token::Type::Scolon)) {
            condition = createExpression();
        }
    } else {
        throwError("Incorrect for loop syntax", forToken.line);
    }

    std::unique_ptr<NodeAST> increment = nullptr;
    if (match(Token::Type::Scolon)) {
        index++; // consume semicolon
        if (!match(Token::Type::Scolon)) {
            increment = createStatement();
        }
    } else {
        throwError("Incorrect for loop syntax", forToken.line);
    }

    consumeSNI();
    std::unique_ptr<NodeAST> forBlock = createBlock();

    return std::make_unique<NodeAST>(NodeAST(forToken, std::move(condition), std::move(forBlock), std::move(initial), std::move(increment)));
}

// -----------------------------------------------------------------------------
// Create Declaration Node
// -----------------------------------------------------------------------------
// Parses variable declaration.
// left  -> variable
// right -> optional assigned expression
// Returns: unique pointer to abstract syntax tree node.
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::createDeclarationNode() {
    index++;

    if (!match(Token::Type::Var)) {
        throwError("Declaration without variable", tokens[index].line, tokens[index].position);
    }

    std::unique_ptr<NodeAST> left = parsePrimary();

    if (match(Token::Type::NewL)) {
        return std::make_unique<NodeAST>(NodeAST(Token::Type::Dec, std::move(left)));
    }

    if (match(Token::Type::Assign)) {
        index++; // skip =
        std::unique_ptr<NodeAST> right = createExpression();
        return std::make_unique<NodeAST>(NodeAST(Token::Type::Dec, std::move(left), std::move(right)));
    }
    
    throwError("Incorrect declaration statement", tokens[index].line);
}

// -----------------------------------------------------------------------------
// Create Assignment Node
// -----------------------------------------------------------------------------
// Parses assignment expression.
// left  -> variable (must be assignable)
// right -> assigned value
// Throws: error if left side is not a valid l-value.
// Returns: unique pointer to abstract syntax tree node.
// -----------------------------------------------------------------------------
std::unique_ptr<Parser::NodeAST> Parser::createAssignmentNode(std::unique_ptr<NodeAST> left) {
    if (left->token.type == Token::Type::Var) {
        Token currToken = tokens[index];
        index++;
        std::unique_ptr<NodeAST> right = createExpression();
        return std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
    } else {
        throwError("Incorrect left value", tokens[index].line);
    }
}

// -----------------------------------------------------------------------------
// Consume SNI (Semicolon, NewLine, Indentation)
// -----------------------------------------------------------------------------
// Ensures and consumes the sequence:
// ";" -> NewLine -> Indentation
// Used to enforce block structure.
// Throws: error if any required token is missing
// -----------------------------------------------------------------------------
void Parser::consumeSNI() {
    if (!match(Token::Type::Scolon)) {
        throwError("\";\" is missing", tokens[index].line, tokens[index].position);
    }
    index++; // consume ";"

    if (!match(Token::Type::NewL)) {
        throwError("New line is missing", tokens[index].line, tokens[index].position);
    }
    index++; // consume NewL

    if (!match(Token::Type::Ind)) {
        throwError("Indentation is missing", tokens[index].line, tokens[index].position);
    }
    index++; // consume Ind
}