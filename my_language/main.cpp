#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "ir.h"
#include "vm.h"

int main() {
    Lexer lexer("code.mylang");
    lexer.printTokens();

    Parser parser(lexer.tokens);

    IR irgenerator(parser.ASTroot);
    irgenerator.print();

    VM vm(irgenerator.instructions);

    for (int i = 0; i < vm.registers.size(); i++) {
        std::cout << "(r" << i << "=" << vm.registers[i] << "),";
    }
    
    std::cout << std::endl;
    return 0;
}