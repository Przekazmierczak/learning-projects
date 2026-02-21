#include "lexer.h"

void Lexer::lex() {
    std::ifstream file(code);
    std::string current;
    std::string line;
    std::vector<int> indents;

    indents.push_back(0);
    int currLine = 1;
    while (std::getline(file, line)) {
        int currPosition = 1;
        int index = 0;

        // Check current indentation
        while (index < line.size() && line[index] == ' ') {
            index++;
        };

        // Ignore empty line
        if (index >= line.size()) continue;

        // Add Ind, Ded if indantation changed
        if (indents.empty() || index > indents.back()) {
            indents.push_back(index);
            tokens.push_back(Token(Token::Type::Ind, currLine, 1));
        } else if (index < indents.back()) {
            while (index != indents.back()) {
                indents.pop_back();
                tokens.push_back(Token(Token::Type::Ded, currLine, 1));
                if (indents.empty()) {
                    throwError("Incorrect indentation", currLine);
                }
            }
        }
        currPosition += index;
        
        for (; index < line.size(); index++) {
            switch (line[index]) {

                case ' ':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    break;
                
                case '+':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Add ,currLine, currPosition));
                    break;
                
                case '-':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Sub ,currLine, currPosition));
                    break;
                
                case '*':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Mul ,currLine, currPosition));
                    break;
                
                case '/':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Div ,currLine, currPosition));
                    break;
                
                case '(':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Lpar ,currLine, currPosition));
                    break;
                
                case ')':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Rpar ,currLine, currPosition));
                    break;
                
                case '=':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    if (index + 1 < line.size() && line[index + 1] == '=') {
                        tokens.push_back(Token(Token::Type::CmpEq ,currLine, currPosition));
                        index++;
                        currPosition++;
                    } else {
                        tokens.push_back(Token(Token::Type::Assign ,currLine, currPosition));
                    }
                    break;
                
                case '!':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    if (index + 1 < line.size() && line[index + 1] == '=') {
                        tokens.push_back(Token(Token::Type::CmpNEq ,currLine, currPosition));
                        index++;
                        currPosition++;
                    } else {
                        throwError("Invalid symbol \"!\"", currLine, currPosition);
                    }
                    break;
                
                case '>':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    if (index + 1 < line.size() && line[index + 1] == '=') {
                        tokens.push_back(Token(Token::Type::CmpGtEq ,currLine, currPosition));
                        index++;
                        currPosition++;
                    } else {
                        tokens.push_back(Token(Token::Type::CmpGt ,currLine, currPosition));
                    }
                    break;
                
                case '<':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    if (index + 1 < line.size() && line[index + 1] == '=') {
                        tokens.push_back(Token(Token::Type::CmpLsEq ,currLine, currPosition));
                        index++;
                        currPosition++;
                    } else {
                        tokens.push_back(Token(Token::Type::CmpLs ,currLine, currPosition));
                    }
                    break;
                
                case ';':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Scolon ,currLine, currPosition));
                    break;
                
                case '&':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    if (index + 1 < line.size() && line[index + 1] == '&') {
                        tokens.push_back(Token(Token::Type::And ,currLine, currPosition));
                        index++;
                        currPosition++;
                    } else {
                        throwError("Invalid symbol \"&\"", currLine, currPosition);
                    }
                    break;
                
                case '|':
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    if (index + 1 < line.size() && line[index + 1] == '|') {
                        tokens.push_back(Token(Token::Type::Or ,currLine, currPosition));
                        index++;
                        currPosition++;
                    } else {
                        throwError("Invalid symbol \"|\"", currLine, currPosition);
                    }
                    break;

                default:
                    current += line[index];
                    break;
                }
            currPosition++;
        }

        if (!current.empty()) pushNonOperand(currLine, currPosition, current);
        currPosition += current.size();
        current.clear();
        
        tokens.push_back(Token(Token::Type::NewL ,currLine, currPosition));
        currLine++;
    }

    // Add missing Ded to close indentation
    while (indents.size() > 1) {
        indents.pop_back();
        tokens.push_back(Token(Token::Type::Ded, currLine, 1));
    }
}

void Lexer::pushNonOperand(int currLine, int currPosition, std::string& current) {
    currPosition = currPosition - current.size();
    if (current == "if") {
        tokens.push_back(Token(Token::Type::If, currLine, currPosition));
    } else if (current == "else") {
        tokens.push_back(Token(Token::Type::Else, currLine, currPosition));
    } else if (current == "while") {
        tokens.push_back(Token(Token::Type::While, currLine, currPosition));
    } else if (current == "dec") {
        tokens.push_back(Token(Token::Type::Dec, currLine, currPosition));
    } else if (current == "for") {
        tokens.push_back(Token(Token::Type::For, currLine, currPosition));
    } else if (current == "and") {
        tokens.push_back(Token(Token::Type::And, currLine, currPosition));
    } else if (current == "or") {
        tokens.push_back(Token(Token::Type::Or, currLine, currPosition));
    } else if (current == "true") {
        tokens.push_back(Token(Token::Type::Bool, true, currLine, currPosition));
    } else if (current == "false") {
        tokens.push_back(Token(Token::Type::Bool, false, currLine, currPosition));
    } else if (std::regex_match(current, patternInt)) {
        tokens.push_back(Token(Token::Type::Int, std::stoi(current), currLine, currPosition));
    } else if (std::regex_match(current, patternName)) {
        tokens.push_back(Token(Token::Type::Var, current, currLine, currPosition));
    } else {
        throwError("Incorrect name \"" + current + "\"", currLine, currPosition);
    }
    current.clear();
}

void Lexer::printTokens() {
    for (int i = 0; i < tokens.size(); i++) {
        std::cout << tokens[i] << ", ";
    }
    std::cout << std::endl;
}