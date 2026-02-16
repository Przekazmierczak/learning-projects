#include "vm.h"

void VM::run(const std::vector<IR::OP>& instructions) {
    while (pc < instructions.size()) {
        switch (instructions[pc].operation)
        {
        case IR::OP::Type::Const:
            resizeReg(instructions[pc].dst);
            registers[instructions[pc].dst] = instructions[pc].val;
            pc++;
            break;
        case IR::OP::Type::Add:
            resizeReg(instructions[pc].dst);
            registers[instructions[pc].dst] = registers[instructions[pc].src1] + registers[instructions[pc].src2];
            pc++;
            break;
        case IR::OP::Type::Sub:
            resizeReg(instructions[pc].dst);
            registers[instructions[pc].dst] = registers[instructions[pc].src1] - registers[instructions[pc].src2];
            pc++;
            break;
        case IR::OP::Type::Mul:
            resizeReg(instructions[pc].dst);
            registers[instructions[pc].dst] = registers[instructions[pc].src1] * registers[instructions[pc].src2];
            pc++;
            break;
        case IR::OP::Type::Div:
            resizeReg(instructions[pc].dst);
            if (registers[instructions[pc].src2] == 0) throwError("Divide by zero error");
            registers[instructions[pc].dst] = registers[instructions[pc].src1] / registers[instructions[pc].src2];
            pc++;
            break;
        case IR::OP::Type::Neg:
            resizeReg(instructions[pc].dst);
            registers[instructions[pc].dst] = -registers[instructions[pc].src1];
            pc++;
            break;
        case IR::OP::Type::Assign:
            map[instructions[pc].name] = instructions[pc].dst;
            pc++;
            break;
        case IR::OP::Type::Load:
            if (map.find(instructions[pc].name) != map.end()) {
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = registers[map[instructions[pc].name]];
                pc++;
            } else {
                throwError("Unknown variable name: \"" + instructions[pc].name + "\"");
            }
            break;
        case IR::OP::Type::CmpEq:
            resizeReg(instructions[pc].dst);
            registers[instructions[pc].dst] = int(registers[instructions[pc].src1] == registers[instructions[pc].src2]);
            pc++;
            break;
        case IR::OP::Type::CmpNEq:
            resizeReg(instructions[pc].dst);
            registers[instructions[pc].dst] = int(registers[instructions[pc].src1] != registers[instructions[pc].src2]);
            pc++;
            break;
        case IR::OP::Type::CmpGt:
            resizeReg(instructions[pc].dst);
            registers[instructions[pc].dst] = int(registers[instructions[pc].src1] > registers[instructions[pc].src2]);
            pc++;
            break;
        case IR::OP::Type::CmpLs:
            resizeReg(instructions[pc].dst);
            registers[instructions[pc].dst] = int(registers[instructions[pc].src1] < registers[instructions[pc].src2]);
            pc++;
            break;
        case IR::OP::Type::CmpGtEq:
            resizeReg(instructions[pc].dst);
            registers[instructions[pc].dst] = int(registers[instructions[pc].src1] >= registers[instructions[pc].src2]);
            pc++;
            break;
        case IR::OP::Type::CmpLsEq:
            resizeReg(instructions[pc].dst);
            registers[instructions[pc].dst] = int(registers[instructions[pc].src1] <= registers[instructions[pc].src2]);
            pc++;
            break;
        case IR::OP::Type::Jmp:
            pc = instructions[pc].dst;
            break;
        case IR::OP::Type::JmpZ:
            if (registers[instructions[pc].src1] == 0) {
                pc = instructions[pc].dst;
            } else {
                pc++;
            }
            break;
        case IR::OP::Type::JmpNZ:
            if (registers[instructions[pc].src1] != 0) {
                pc = instructions[pc].dst;
            } else {
                pc++;
            }
            break;
        default:
            throwError("Unknown operation");
            break;
        }
    }
}

void VM::resizeReg(int dst) {
    if (dst >= registers.size()) {
        registers.resize(dst + 1);
    }
}