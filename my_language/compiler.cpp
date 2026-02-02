#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stack>
#include <memory>

struct Source {
    std::vector<std::string> source;
};

enum class TokenType {
    Int,
    Add,
    Sub,
    Mul,
    Div,
    Lpar,
    Rpar
};

struct Token {
    TokenType type;
    bool operant = false;
    int precedence = 0;
    int val = 0;
};

struct NodeAST {
    Token token;
    std::unique_ptr<NodeAST> left;
    std::unique_ptr<NodeAST> right;

    NodeAST(Token newToken) : token(newToken) {}
};

Source lex(std::string code) {
    Source result;
    std::ifstream file(code);
    std::string line;
    std::string current;

    while (std::getline(file, line)) {
        for (char c : line) {
        if (c == ' ') {
            if (!current.empty()) {
                result.source.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }

    if (!current.empty())
        result.source.push_back(current);
    }
    return result;
}

std::vector<Token> parse(Source source) {
    std::vector<Token> tokens;
    for (int i = 0; i < source.source.size(); i++) {
        Token current;
        if (source.source[i] == "+") {
            current.type = TokenType::Add;
            current.operant = true;
            current.precedence = 20;
        } else if (source.source[i] == "-") {
            current.type = TokenType::Sub;
            current.operant = true;
            current.precedence = 20;
        } else if (source.source[i] == "*") {
            current.type = TokenType::Mul;
            current.operant = true;
            current.precedence = 30;
        } else if (source.source[i] == "/") {
            current.type = TokenType::Div;
            current.operant = true;
            current.precedence = 30;
        } else if (source.source[i] == "(") {
            current.type = TokenType::Lpar;
            current.operant = true;
            current.precedence = 40;
        } else if (source.source[i] == ")") {
            current.type = TokenType::Rpar;
            current.operant = true;
            current.precedence = 10;
        } else {
            current.type = TokenType::Int;
            current.val = std::stoi(source.source[i]);
        }
        tokens.push_back(current);
    }
    return tokens;
}

void reduce(std::stack<std::unique_ptr<NodeAST>>& stackValues, std::stack<Token>& stackOperants) {
    std::unique_ptr<NodeAST> current = std::make_unique<NodeAST>(stackOperants.top());
    stackOperants.pop();

    current->right = std::move(stackValues.top());
    stackValues.pop();
    current->left = std::move(stackValues.top());
    stackValues.pop();

    stackValues.push(std::move(current));
}

std::unique_ptr<NodeAST>createAST (std::vector<Token> tokens) {
    std::stack<std::unique_ptr<NodeAST>> stackValues;
    std::stack<Token> stackOperants;

    for (int i = 0; i < tokens.size(); i++) {
        if (!tokens[i].operant) {
            stackValues.push(std::make_unique<NodeAST>(tokens[i]));
        }
        
        else if (tokens[i].type == TokenType::Lpar) {
            stackOperants.push(tokens[i]);
        }
        
        else if (tokens[i].type == TokenType::Rpar) {
            while (!stackOperants.empty() && stackOperants.top().type != TokenType::Lpar) {
                reduce(stackValues, stackOperants);
            }
            stackOperants.pop();
        }
        
        else {
            while (!stackOperants.empty() && stackOperants.top().type != TokenType::Lpar && stackOperants.top().precedence >= tokens[i].precedence) {
                reduce(stackValues, stackOperants);
            }
            
            stackOperants.push(tokens[i]);
        }
    }
    while (!stackOperants.empty()) {
        reduce(stackValues, stackOperants);
    }

    return std::move(stackValues.top());
}

int interpreter(const std::unique_ptr<NodeAST>& node) {
    if (node->token.type == TokenType::Int) {
        return node->token.val;
    }

    int left = interpreter(node->left);
    int right = interpreter(node->right);

    if (node->token.type == TokenType::Add) {
        return left + right;
    } else if (node->token.type == TokenType::Sub) {
        return left - right;
    } else if (node->token.type == TokenType::Mul) {
        return left * right;
    } else if (node->token.type == TokenType::Div) {
        return left / right;
    }

    // return 0;
}

int main() {
    Source source = lex("test.elil");
    std::vector<Token> tokens = parse(source);
    std::unique_ptr<NodeAST> ast = createAST(tokens);

    // std::cout << ast->token.precedence << std::endl;
    // std::cout << ast->left->left->token.val << std::endl;
    // std::cout << ast->left->right->token.val << std::endl;
    // std::cout << ast->right->token.val << std::endl;
    // std::cout << ast->right->left->token.val << std::endl;
    // std::cout << ast->right->right->token.val << std::endl;

    std::cout << interpreter(ast) << std::endl;

    return 0;
}