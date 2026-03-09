#include "lexer.h"

void Lexer::lex() {
    std::ifstream file(code);
    std::string line;
    std::vector<int> indents;

    indents.push_back(0);

    while (std::getline(file, line)) {
        int currPosition = 1;
        index = 0;

        // Check current indentation
        while (index < line.size() && line[index] == ' ') {
            index++;
        };

        // Ignore empty line
        if (index >= line.size()) {
            currLine++;
            continue;
        }

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
            if (std::isdigit(line[index])) {
                scanNumeric(currPosition, line);
                continue;
            }
            
            if (std::isalpha(line[index]) || line[index] == '_') {
                scanIdentifier(currPosition, line);
                continue;
            }

            scanOperant(currPosition, line);

            currPosition++;
        }

        // if (!current.empty()) pushNonOperand(currLine, currPosition, current);
        // currPosition += current.size();
        // current.clear();
        
        tokens.push_back(Token(Token::Type::NewL ,currLine, currPosition));
        currLine++;
    }

    // Add missing Ded to close indentation
    while (indents.size() > 1) {
        indents.pop_back();
        tokens.push_back(Token(Token::Type::Ded, currLine, 1));
    }
}

void Lexer::scanNumeric(int currPosition, const std::string& line) {
    std::string numStr;
    numStr += line[index];
    while (index + 1 < line.size() && std::isdigit(line[index + 1])) {
        index++;
        numStr += line[index];
    }
    tokens.push_back(Token(Token::Type::Int, std::stoi(numStr), currLine, currPosition));
}

void Lexer::scanIdentifier(int currPosition, const std::string& line) {
    std::string indentifier;
    indentifier += line[index];
    while (index + 1 < line.size() && (std::isalnum(line[index + 1]) || line[index] == '_')) {
        index++;
        indentifier += line[index];
    }
    pushIdentifier(indentifier, currPosition);
}

void Lexer::scanOperant(int currPosition, const std::string& line) {
    switch (line[index]) {

        case ' ':
            break;
        
        case '+':
            tokens.push_back(Token(Token::Type::Add ,currLine, currPosition));
            break;
        
        case '-':
            tokens.push_back(Token(Token::Type::Sub ,currLine, currPosition));
            break;
        
        case '*':
            tokens.push_back(Token(Token::Type::Mul ,currLine, currPosition));
            break;
        
        case '/':
            tokens.push_back(Token(Token::Type::Div ,currLine, currPosition));
            break;
        
        case '(':
            tokens.push_back(Token(Token::Type::Lpar ,currLine, currPosition));
            break;
        
        case ')':
            tokens.push_back(Token(Token::Type::Rpar ,currLine, currPosition));
            break;
        
        case '=':
            if (index + 1 < line.size() && line[index + 1] == '=') {
                tokens.push_back(Token(Token::Type::CmpEq ,currLine, currPosition));
                index++;
                currPosition++;
            } else {
                tokens.push_back(Token(Token::Type::Assign ,currLine, currPosition));
            }
            break;
        
        case '!':
            if (index + 1 < line.size() && line[index + 1] == '=') {
                tokens.push_back(Token(Token::Type::CmpNEq ,currLine, currPosition));
                index++;
                currPosition++;
            } else {
                throwError("Invalid symbol \"!\"", currLine, currPosition);
            }
            break;
        
        case '>':
            if (index + 1 < line.size() && line[index + 1] == '=') {
                tokens.push_back(Token(Token::Type::CmpGtEq ,currLine, currPosition));
                index++;
                currPosition++;
            } else {
                tokens.push_back(Token(Token::Type::CmpGt ,currLine, currPosition));
            }
            break;
        
        case '<':
            if (index + 1 < line.size() && line[index + 1] == '=') {
                tokens.push_back(Token(Token::Type::CmpLsEq ,currLine, currPosition));
                index++;
                currPosition++;
            } else {
                tokens.push_back(Token(Token::Type::CmpLs ,currLine, currPosition));
            }
            break;
        
        case ';':
            tokens.push_back(Token(Token::Type::Scolon ,currLine, currPosition));
            break;
        
        case '&':
            if (index + 1 < line.size() && line[index + 1] == '&') {
                tokens.push_back(Token(Token::Type::And ,currLine, currPosition));
                index++;
                currPosition++;
            } else {
                throwError("Invalid symbol \"&\"", currLine, currPosition);
            }
            break;
        
        case '|':
            if (index + 1 < line.size() && line[index + 1] == '|') {
                tokens.push_back(Token(Token::Type::Or ,currLine, currPosition));
                index++;
                currPosition++;
            } else {
                throwError("Invalid symbol \"|\"", currLine, currPosition);
            }
            break;

        case ',':
            tokens.push_back(Token(Token::Type::Comma ,currLine, currPosition));
            break;

        default:
            throwError(std::string("Incorrect symbol \"") + line[index] + "\"", currLine, currPosition);
        }
}

void Lexer::pushIdentifier(std::string identifier, int currPosition) {
    currPosition = currPosition - identifier.size();
    if (identifier == "if") {
        tokens.push_back(Token(Token::Type::If, currLine, currPosition));
    } else if (identifier == "else") {
        tokens.push_back(Token(Token::Type::Else, currLine, currPosition));
    } else if (identifier == "while") {
        tokens.push_back(Token(Token::Type::While, currLine, currPosition));
    } else if (identifier == "dec") {
        tokens.push_back(Token(Token::Type::Dec, currLine, currPosition));
    } else if (identifier == "for") {
        tokens.push_back(Token(Token::Type::For, currLine, currPosition));
    } else if (identifier == "and") {
        tokens.push_back(Token(Token::Type::And, currLine, currPosition));
    } else if (identifier == "or") {
        tokens.push_back(Token(Token::Type::Or, currLine, currPosition));
    } else if (identifier == "true") {
        tokens.push_back(Token(Token::Type::Bool, true, currLine, currPosition));
    } else if (identifier == "false") {
        tokens.push_back(Token(Token::Type::Bool, false, currLine, currPosition));
    } else if (identifier == "fun") {
        tokens.push_back(Token(Token::Type::Fun, currLine, currPosition));
    } else if (identifier == "return") {
        tokens.push_back(Token(Token::Type::Ret, currLine, currPosition));
    } else {
        tokens.push_back(Token(Token::Type::Var, identifier, currLine, currPosition));
    }
}

void Lexer::printTokens() {
    for (int i = 0; i < tokens.size(); i++) {
        std::cout << tokens[i] << ", ";
    }
    std::cout << std::endl;
}