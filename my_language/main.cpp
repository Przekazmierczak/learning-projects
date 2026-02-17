#include "lexer.h"
#include "parser.h"
#include "ir.h"
#include "vm.h"
#include "assert.h"

// Ducklingua

int tests();

int main() {
    Lexer lexer("code.duck");
    lexer.printTokens();

    Parser parser(lexer.tokens);

    IR irgenerator(parser.ASTroot);
    irgenerator.print();

    VM vm(irgenerator.instructions);

    for (int i = 0; i < vm.registers.size(); i++) {
        std::cout << "(r" << i << "=" << vm.registers[i] << "),";
    }
    std::cout << std::endl;

    return tests();
}

int tests() {
    Lexer lexer1("test1.duckbugger");
    Parser parser1(lexer1.tokens);
    IR irgenerator1(parser1.ASTroot);
    VM vm1(irgenerator1.instructions);
    int res1[] = {1, 23, 24, 45, 4, 41, 4, 5, 20, 7, 7, 1, 2, -2, 3, -3, 3};

    for (int i = 0; i < vm1.registers.size(); i++) {
        assert(vm1.registers[i] == res1[i]);
    }

    Lexer lexer2("test2.duckbugger");
    Parser parser2(lexer2.tokens);
    IR irgenerator2(parser2.ASTroot);
    VM vm2(irgenerator2.instructions);
    int res2[] = {4, 5, 0, 0, 0, 0, 0, 0, 3, 4, 1, 3, 3, 1, 0, 1, 4, 3, 1};

    for (int i = 0; i < vm2.registers.size(); i++) {
        assert(vm2.registers[i] == res2[i]);
    }

    Lexer lexer3("test3.duckbugger");
    Parser parser3(lexer3.tokens);
    IR irgenerator3(parser3.ASTroot);
    VM vm3(irgenerator3.instructions);
    int res3[] = {1, 5, 5, 0, 0, 0, 4, 1, 5, 5};
    
    for (int i = 0; i < vm3.registers.size(); i++) {
        assert(vm3.registers[i] == res3[i]);
    }
    std::cout << "All test passed!" << std::endl;
    return 0;
}