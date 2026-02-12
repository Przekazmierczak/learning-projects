#ifndef VM_H
#define VM_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <regex>

#include "ir.h"

struct VM {
    std::vector<int> registers;
    std::unordered_map<std::string, int> map;

    int pc = 0;
    int result;

    VM(std::vector<IRInstruction> instructions) {
        run(instructions);
    }

    void resizeReg(int dst) {
        if (dst >= registers.size()) {
            registers.resize(dst + 1);
        }
    }

    void run(const std::vector<IRInstruction>& instructions) {
        while (pc < instructions.size()) {
            switch (instructions[pc].operation)
            {
            case IRInstruction::OP::Const:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = instructions[pc].val;
                pc++;
                break;
            case IRInstruction::OP::Add:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = registers[instructions[pc].src1] + registers[instructions[pc].src2];
                pc++;
                break;
            case IRInstruction::OP::Sub:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = registers[instructions[pc].src1] - registers[instructions[pc].src2];
                pc++;
                break;
            case IRInstruction::OP::Mul:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = registers[instructions[pc].src1] * registers[instructions[pc].src2];
                pc++;
                break;
            case IRInstruction::OP::Div:
                resizeReg(instructions[pc].dst);
                if (registers[instructions[pc].src2] == 0) throwError("Divide by zero error");
                registers[instructions[pc].dst] = registers[instructions[pc].src1] / registers[instructions[pc].src2];
                pc++;
                break;
            case IRInstruction::OP::Neg:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = -registers[instructions[pc].src1];
                pc++;
                break;
            case IRInstruction::OP::Assign:
                map[instructions[pc].name] = instructions[pc].dst;
                pc++;
                break;
            case IRInstruction::OP::Load:
                if (map.find(instructions[pc].name) != map.end()) {
                    resizeReg(instructions[pc].dst);
                    registers[instructions[pc].dst] = registers[map[instructions[pc].name]];
                    pc++;
                } else {
                    throwError("Unknown variable name: \"" + instructions[pc].name + "\"");
                }
                break;
            case IRInstruction::OP::Cmp:
                registers[instructions[pc].dst] = registers[instructions[pc].src1] - registers[instructions[pc].src2];
                pc++;
                break;
            case IRInstruction::OP::Jmp:
                pc = instructions[pc].dst;
                break;
            case IRInstruction::OP::JmpZ:
                if (registers[instructions[pc].src1] != 0) {
                    pc++;
                } else {
                    pc = instructions[pc].dst;
                }
                break;
            default:
                throwError("Unknown operation");
                break;
            }
        }
    }
};

#endif