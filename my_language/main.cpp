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

    VM vm(irgenerator.instructions, irgenerator.functionsInstructions, irgenerator.functionsMap);

    for (int i = 0; i < vm.registers.size(); i++) {
        if (vm.registers[i].type == VM::Type::Int) {
            std::cout << "r" << i << "=[int, " << vm.registers[i].i << "],";
        }
        if (vm.registers[i].type == VM::Type::Bool) {
            std::cout << "r" << i << "=[bool, " << vm.registers[i].b << "],";
        }
        if (vm.registers[i].type == VM::Type::NaN) {
            std::cout << "r" << i << "=[Nan],";
        }
    }
    std::cout << std::endl;

    return tests();
}

int tests() {
    Lexer lexer1("test1.duckbugger");
    Parser parser1(lexer1.tokens);
    IR irgenerator1(parser1.ASTroot);
    VM vm1(irgenerator1.instructions, irgenerator1.functionsInstructions, irgenerator1.functionsMap);

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
    VM vm2(irgenerator2.instructions, irgenerator2.functionsInstructions, irgenerator2.functionsMap);

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
    VM vm3(irgenerator3.instructions, irgenerator3.functionsInstructions, irgenerator3.functionsMap);

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

    Lexer lexer4("test4.duckbugger");
    Parser parser4(lexer4.tokens);
    IR irgenerator4(parser4.ASTroot);
    VM vm4(irgenerator4.instructions, irgenerator4.functionsInstructions, irgenerator4.functionsMap);

    assert(vm4.registers[vm4.registers.size() - 1] == VM::Variable(4));

    Lexer lexer5("test5.duckbugger");
    Parser parser5(lexer5.tokens);
    IR irgenerator5(parser5.ASTroot);
    VM vm5(irgenerator5.instructions, irgenerator5.functionsInstructions, irgenerator5.functionsMap);

    assert(vm5.registers[0] == VM::Variable(10));
    
    Lexer lexer6("test6.duckbugger");
    Parser parser6(lexer6.tokens);
    IR irgenerator6(parser6.ASTroot);
    VM vm6(irgenerator6.instructions, irgenerator6.functionsInstructions, irgenerator6.functionsMap);

    assert(vm6.registers[0] == VM::Variable(56));

    Lexer lexer7("test7.duckbugger");
    Parser parser7(lexer7.tokens);
    IR irgenerator7(parser7.ASTroot);
    VM vm7(irgenerator7.instructions, irgenerator7.functionsInstructions, irgenerator7.functionsMap);

    assert(vm7.registers[0] == VM::Variable(55));
    
    std::cout << "All test passed!" << std::endl;

    return 0;
}