#ifndef IR_H
#define IR_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <regex>

#include "token.h"
#include "parser.h"

struct IRInstruction {
    enum class OP {
        Const,
        Add, Sub, Mul, Div,
        Neg,
        Assign, Load,
        Cmp,
        Jmp, JmpZ, JmpNZ
    };

    OP operation;
    int dst;
    int src1;
    int src2;
    int val;
    std::string name;
};

std::ostream& operator << (std::ostream& cout, IRInstruction& inst)
{
    if (inst.operation == IRInstruction::OP::Const) {
        cout << "Move r" << inst.dst << ", " << inst.val << std::endl;
    } else if (inst.operation == IRInstruction::OP::Neg) {
        cout << "Neg" << " r" << inst.dst << ", r" << inst.src1 << std::endl;
    } else if (inst.operation == IRInstruction::OP::Assign) {
        cout << "Assign \"" << inst.name << "\", r" << inst.dst << std::endl;
    } else if (inst.operation == IRInstruction::OP::Load) {
        cout << "Load r" << inst.dst << ", \"" << inst.name << "\"" << std::endl;
    } else if (inst.operation == IRInstruction::OP::Jmp) {
        cout << "Jmp pc" << inst.dst << std::endl;
    } else if (inst.operation == IRInstruction::OP::JmpZ) {
        cout << "JmpZ pc" << inst.dst << ", r" << inst.src1 << std::endl;
    } else {
        std::string op;
        if (inst.operation == IRInstruction::OP::Add) op = "Add";
        else if (inst.operation == IRInstruction::OP::Sub) op = "Sub";
        else if (inst.operation == IRInstruction::OP::Mul) op = "Mul";
        else if (inst.operation == IRInstruction::OP::Div) op = "Div";
        else if (inst.operation == IRInstruction::OP::Cmp) op = "Cmp";
        cout << op << " r" << inst.dst << ", r" << inst.src1 << ", r" << inst.src2 << std::endl;
    }
    return cout;
}

struct IRGenerator {
    struct RegisterIndex {
        int curr = 0;

        int getNext() {
            return curr++;
        }
    };

    IRGenerator(const std::unique_ptr<Parser::NodeAST>& node) {
        generate(node);
    }

    std::vector<IRInstruction> instructions;
    RegisterIndex index;

    int generate(const std::unique_ptr<Parser::NodeAST>& node) {
        IRInstruction newInstruction;
        if (node->token.type == Token::Type::Int) {
            newInstruction.operation = IRInstruction::OP::Const;
            newInstruction.dst = index.getNext();
            newInstruction.val = node->token.val;
            instructions.push_back(newInstruction);
        } else if (node->token.type == Token::Type::Block) {
            for (int i = 0; i < node->statements.size(); i++) {
                generate(node->statements[i]);
            }
            return instructions.size();
        } else if (node->token.type == Token::Type::Neg) {
            newInstruction.operation = IRInstruction::OP::Neg;
            newInstruction.src1 = generate(node->left);
            newInstruction.dst = index.getNext();
            instructions.push_back(newInstruction);
        } else if (node->token.type == Token::Type::Assign) {
            newInstruction.operation = IRInstruction::OP::Assign;
            newInstruction.dst = generate(node->right);
            newInstruction.name = node->left->token.name;
            instructions.push_back(newInstruction);
            return -1;
        } else if (node->token.type == Token::Type::Var) {
            newInstruction.operation = IRInstruction::OP::Load;
            newInstruction.dst = index.getNext();
            newInstruction.name = node->token.name;
            instructions.push_back(newInstruction);
        } else if (node->token.type == Token::Type::If) {
            addIfInstructions(node);
            return -1;
        } else {
            if (node->token.type == Token::Type::Add) newInstruction.operation = IRInstruction::OP::Add;
            else if (node->token.type == Token::Type::Sub) newInstruction.operation = IRInstruction::OP::Sub;
            else if (node->token.type == Token::Type::Mul) newInstruction.operation = IRInstruction::OP::Mul;
            else if (node->token.type == Token::Type::Div) newInstruction.operation = IRInstruction::OP::Div;
            newInstruction.src1 = generate(node->left);
            newInstruction.src2 = generate(node->right);
            newInstruction.dst = index.getNext();
            instructions.push_back(newInstruction);
        }
        return newInstruction.dst;
    }

    void addIfInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
        IRInstruction cmp;
        cmp.operation = IRInstruction::OP::Cmp;
        cmp.src1 = generate(node->condition);
        cmp.src2 = addConst(0);;
        cmp.dst = index.getNext();
        instructions.push_back(cmp);

        IRInstruction JmpZ;
        JmpZ.operation = IRInstruction::OP::JmpZ;
        JmpZ.src1 = cmp.dst;
        
        int pc = instructions.size(); // Save Jmpz location
        instructions.push_back(JmpZ);
        instructions[pc].dst = generate(node->left);

        if (node->right) {
            instructions[pc].dst++; // Pass over new Jmp instruction

            IRInstruction Jmp;
            Jmp.operation = IRInstruction::OP::Jmp;
            pc = instructions.size();
            instructions.push_back(Jmp);
            instructions[pc].dst = generate(node->right);
        }
    }

    int addConst(int val) {
        IRInstruction mov;
        mov.operation = IRInstruction::OP::Const;
        mov.dst = index.getNext();
        mov.val = val;
        instructions.push_back(mov);
        return mov.dst;
    }

    void print() {
        for (int i = 0; i < instructions.size(); i++) {
            std::cout << i << ". " << instructions[i];
        }
    }
};

#endif