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
        if (vm.registers[i].type == VM::Type::Int) {
            std::cout << "r" << i << "=[int, " << vm.registers[i].i << "],";
        }
        if (vm.registers[i].type == VM::Type::Bool) {
            std::cout << "r" << i << "=[bool, " << vm.registers[i].b << "],";
        }
    }
    std::cout << std::endl;

    return tests();
}

int tests() {
    Lexer lexer1("test1.duckbugger");
    Parser parser1(lexer1.tokens);
    IR irgenerator1(parser1.ASTroot);
    VM vm1(irgenerator1.instructions);

    VM::Variable res1[] = {
        VM::Variable(1),
        VM::Variable(23),
        VM::Variable(24),
        VM::Variable(45),
        VM::Variable(4),
        VM::Variable(41),
        VM::Variable(4),
        VM::Variable(5),
        VM::Variable(20),
        VM::Variable(7),
        VM::Variable(7),
        VM::Variable(1),
        VM::Variable(2),
        VM::Variable(-2),
        VM::Variable(3),
        VM::Variable(-3),
        VM::Variable(3)
    };

    Lexer lexer2("test2.duckbugger");
    Parser parser2(lexer2.tokens);
    IR irgenerator2(parser2.ASTroot);
    VM vm2(irgenerator2.instructions);

    VM::Variable res2[] = {
        VM::Variable(4),
        VM::Variable(5),
        VM::Variable(false),
        VM::Variable(),
        VM::Variable(),
        VM::Variable(),
        VM::Variable(3),
        VM::Variable(4),
        VM::Variable(true),
        VM::Variable(3),
        VM::Variable(3),
        VM::Variable(true),
        VM::Variable(4),
        VM::Variable(3),
        VM::Variable(true)
    };

    Lexer lexer3("test3.duckbugger");
    Parser parser3(lexer3.tokens);
    IR irgenerator3(parser3.ASTroot);
    VM vm3(irgenerator3.instructions);

    VM::Variable res3[] = {
        VM::Variable(1),
        VM::Variable(5),
        VM::Variable(5),
        VM::Variable(false),
        VM::Variable(4),
        VM::Variable(1),
        VM::Variable(5),
        VM::Variable(5)
    };

    for (int i = 0; i < vm3.registers.size(); i++) {
        assert(vm3.registers[i] == res3[i]);
    }

    std::cout << "All test passed!" << std::endl;

    return 0;
}