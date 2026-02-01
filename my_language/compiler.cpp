#include <string>
#include <vector>
#include <fstream>
#include <iostream>

struct Source {
    std::vector<std::string> source;
};

enum class TokenType {
    Int,
    Add,
    Sub,
    Mul,
    Div
};

struct Token {
    TokenType type;
    int val;
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
        } else if (source.source[i] == "-") {
            current.type = TokenType::Sub;
        } else if (source.source[i] == "*") {
            current.type = TokenType::Mul;
        } else if (source.source[i] == "/") {
            current.type = TokenType::Div;
        } else {
            current.type = TokenType::Int;
            current.val = std::stoi(source.source[i]);
        }
        tokens.push_back(current);
    }
    return tokens;
}

int main() {
    Source source = lex("test.elil");
    std::vector<Token> tokens = parse(source);

    int result;

    if (tokens[0].type == TokenType::Add) {
        if (tokens[1].type == TokenType::Int && tokens[2].type == TokenType::Int) {
            result = tokens[1].val + tokens[2].val;
        }
    } else if (tokens[0].type == TokenType::Sub) {
        if (tokens[1].type == TokenType::Int && tokens[2].type == TokenType::Int) {
            result = tokens[1].val - tokens[2].val;
        }
    } else if (tokens[0].type == TokenType::Mul) {
        if (tokens[1].type == TokenType::Int && tokens[2].type == TokenType::Int) {
            result = tokens[1].val * tokens[2].val;
        }
    } else if (tokens[0].type == TokenType::Div) {
        if (tokens[1].type == TokenType::Int && tokens[2].type == TokenType::Int) {
            result = tokens[1].val / tokens[2].val;
        }
    }    
    std::cout << result << std::endl;

    return 0;
}