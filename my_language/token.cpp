#include "token.h"

std::ostream& operator << (std::ostream& cout, const Token& token)
{
    if (token.type == Token::Type::Int) {
        cout << "(" << names[static_cast<int>(token.type)] << ", " << token.val << ", [" << token.line << ", " << token.position << "])";
    } else if (token.type == Token::Type::String || token.type == Token::Type::Var) {
        cout << "(" << names[static_cast<int>(token.type)] << ", \"" << token.name << "\", [" << token.line << ", " << token.position << "])";
    } else {
        cout << "(" << names[static_cast<int>(token.type)] << ", [" << token.line << ", " << token.position << "])";
    }
    return cout;
}