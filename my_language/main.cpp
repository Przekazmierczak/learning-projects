#include "lexer.h"
#include "parser.h"
#include "ir.h"
#include "vm.h"
#include "assert.h"
#include "variable.h"

// Ducklingua

int tests();

int main() {
    Lexer lexer("code.duck");
    lexer.printTokens();

    Parser parser(lexer.tokens);

    IR irgenerator(parser.ASTroot);
    irgenerator.print();

    VM vm(irgenerator.instructions, irgenerator.functionsInstructions, irgenerator.functionsMetaMap);

    for (int i = 0; i < vm.registers.size(); i++) {
        if (vm.registers[i].isInt()) {
            std::cout << "r" << i << "=[int, " << std::get<int>(vm.registers[i].value) << "],";
        }
        if (vm.registers[i].isBool()) {
            std::cout << "r" << i << "=[bool, " << std::get<bool>(vm.registers[i].value) << "],";
        }
        if (vm.registers[i].isNaN()) {
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
    VM vm1(irgenerator1.instructions, irgenerator1.functionsInstructions, irgenerator1.functionsMetaMap);

    Variable res1[] = {
        Variable(1),
        Variable(23),
        Variable(24),
        Variable(45),
        Variable(4),
        Variable(41),
        Variable(4),
        Variable(5),
        Variable(20),
        Variable(7),
        Variable(7),
        Variable(1),
        Variable(2),
        Variable(-2),
        Variable(3),
        Variable(-3),
        Variable(3)
    };

    Lexer lexer2("test2.duckbugger");
    Parser parser2(lexer2.tokens);
    IR irgenerator2(parser2.ASTroot);
    VM vm2(irgenerator2.instructions, irgenerator2.functionsInstructions, irgenerator2.functionsMetaMap);

    Variable res2[] = {
        Variable(4),
        Variable(5),
        Variable(false),
        Variable(),
        Variable(),
        Variable(),
        Variable(3),
        Variable(4),
        Variable(true),
        Variable(3),
        Variable(3),
        Variable(true),
        Variable(4),
        Variable(3),
        Variable(true)
    };

    Lexer lexer3("test3.duckbugger");
    Parser parser3(lexer3.tokens);
    IR irgenerator3(parser3.ASTroot);
    VM vm3(irgenerator3.instructions, irgenerator3.functionsInstructions, irgenerator3.functionsMetaMap);

    Variable res3[] = {
        Variable(1),
        Variable(5),
        Variable(5),
        Variable(false),
        Variable(4),
        Variable(1),
        Variable(5),
        Variable(5)
    };

    for (int i = 0; i < vm3.registers.size(); i++) {
        assert(vm3.registers[i] == res3[i]);
    }

    Lexer lexer4("test4.duckbugger");
    Parser parser4(lexer4.tokens);
    IR irgenerator4(parser4.ASTroot);
    VM vm4(irgenerator4.instructions, irgenerator4.functionsInstructions, irgenerator4.functionsMetaMap);

    assert(vm4.registers.back() == Variable(4));

    Lexer lexer5("test5.duckbugger");
    Parser parser5(lexer5.tokens);
    IR irgenerator5(parser5.ASTroot);
    VM vm5(irgenerator5.instructions, irgenerator5.functionsInstructions, irgenerator5.functionsMetaMap);

    assert(vm5.registers[0] == Variable(10));
    
    Lexer lexer6("test6.duckbugger");
    Parser parser6(lexer6.tokens);
    IR irgenerator6(parser6.ASTroot);
    VM vm6(irgenerator6.instructions, irgenerator6.functionsInstructions, irgenerator6.functionsMetaMap);

    assert(vm6.registers.back() == Variable(6));

    Lexer lexer7("test7.duckbugger");
    Parser parser7(lexer7.tokens);
    IR irgenerator7(parser7.ASTroot);
    VM vm7(irgenerator7.instructions, irgenerator7.functionsInstructions, irgenerator7.functionsMetaMap);

    assert(vm7.registers[0] == Variable(56));

    Lexer lexer8("test8.duckbugger");
    Parser parser8(lexer8.tokens);
    IR irgenerator8(parser8.ASTroot);
    VM vm8(irgenerator8.instructions, irgenerator8.functionsInstructions, irgenerator8.functionsMetaMap);

    assert(vm8.registers[0] == Variable(55));
    
    std::cout << "All test passed!" << std::endl;

    return 0;
}