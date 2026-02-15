#ifndef IR_H
#define IR_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "parser.h"

struct IR {
    struct OP {
        enum class Type {
            Const,
            Add, Sub, Mul, Div,
            Neg,
            Assign, Load,
            CmpEq, CmpNEq, CmpGt, CmpLs, CmpGtEq, CmpLsEq,
            Jmp, JmpZ, JmpNZ
        };

        Type operation;
        int dst;
        int src1;
        int src2;
        int val;
        std::string name;
    };
    
    struct RegisterIndex {
        int curr = 0;

        int getNext() {
            return curr++;
        }
    };

    std::vector<OP> instructions;
    RegisterIndex index;

    IR(const std::unique_ptr<Parser::NodeAST>& node) {
        generate(node);
    }

    int generate(const std::unique_ptr<Parser::NodeAST>& node);
    void addIfInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addConst(int val);
    void print();
};

std::ostream& operator << (std::ostream& cout, IR::OP& inst);

#endif