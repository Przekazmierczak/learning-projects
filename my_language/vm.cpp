#include "vm.h"

void VM::run(const std::vector<IR::OP>& instructions) {
    while (pc < instructions.size()) {
        switch (instructions[pc].operation) {

            case IR::OP::Type::Int:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst].type = VM::Type::Int;
                registers[instructions[pc].dst].i = instructions[pc].val;
                pc++;
                break;

            case IR::OP::Type::Add:
                resizeReg(instructions[pc].dst);
                if (isInt(registers[instructions[pc].src1]) && isInt(registers[instructions[pc].src2])) {
                    registers[instructions[pc].dst].type = Type::Int;
                    registers[instructions[pc].dst].i = registers[instructions[pc].src1].i + registers[instructions[pc].src2].i;
                } else {
                    throwError("Incorrect types for Add");
                }
                pc++;
                break;

            case IR::OP::Type::Sub:
                resizeReg(instructions[pc].dst);
                if (isInt(registers[instructions[pc].src1]) && isInt(registers[instructions[pc].src2])) {
                    registers[instructions[pc].dst].type = Type::Int;
                    registers[instructions[pc].dst].i = registers[instructions[pc].src1].i - registers[instructions[pc].src2].i;
                } else {
                    throwError("Incorrect types for Sub");
                }
                pc++;
                break;

            case IR::OP::Type::Mul:
                resizeReg(instructions[pc].dst);
                if (isInt(registers[instructions[pc].src1]) && isInt(registers[instructions[pc].src2])) {
                    registers[instructions[pc].dst].type = Type::Int;
                    registers[instructions[pc].dst].i = registers[instructions[pc].src1].i * registers[instructions[pc].src2].i;
                } else {
                    throwError("Incorrect types for Mul");
                }
                pc++;
                break;

            case IR::OP::Type::Div:
                resizeReg(instructions[pc].dst);
                if (isInt(registers[instructions[pc].src1]) && isInt(registers[instructions[pc].src2])) {
                    if (registers[instructions[pc].src2].i == 0) throwError("Divide by zero error");
                    registers[instructions[pc].dst].type = Type::Int;
                    registers[instructions[pc].dst].i = registers[instructions[pc].src1].i / registers[instructions[pc].src2].i;
                } else {
                    throwError("Incorrect types for Div");
                }
                pc++;
                break;

            case IR::OP::Type::Neg:
                resizeReg(instructions[pc].dst);
                if (isInt(registers[instructions[pc].src1])) {
                    registers[instructions[pc].dst].type = Type::Int;
                    registers[instructions[pc].dst].i = -registers[instructions[pc].src1].i;
                } else {
                    throwError("Incorrect types for Div");
                }
                pc++;
                break;

            case IR::OP::Type::Dec:
                if (map.find(instructions[pc].name) != map.end()) {
                    throwError("Variable \"" + instructions[pc].name + "\" is already declared");
                } else {
                    map[instructions[pc].name] = -1;
                }
                pc++;
                break;

            case IR::OP::Type::Assign:
                if (map.find(instructions[pc].name) != map.end()) {
                    map[instructions[pc].name] = instructions[pc].dst;
                } else {
                    throwError("Variable \"" + instructions[pc].name + "\" was never declared");
                }
                pc++;
                break;

            case IR::OP::Type::Load:
                if (map.find(instructions[pc].name) != map.end()) {
                    if (map[instructions[pc].name] == -1) {
                        throwError("Variable \"" + instructions[pc].name + "\" was never initialized");
                    }
                    resizeReg(instructions[pc].dst);
                    registers[instructions[pc].dst] = registers[map[instructions[pc].name]];
                    pc++;
                } else {
                    throwError("Unknown variable name: \"" + instructions[pc].name + "\"");
                }
                break;

            case IR::OP::Type::CmpEq:
                resizeReg(instructions[pc].dst);
                if (isInt(registers[instructions[pc].src1]) && isInt(registers[instructions[pc].src2])) {
                    registers[instructions[pc].dst].type = Type::Bool;
                    registers[instructions[pc].dst].b = registers[instructions[pc].src1].i == registers[instructions[pc].src2].i;
                } else {
                    throwError("Incorrect types for CmpEq");
                }
                pc++;
                break;

            case IR::OP::Type::CmpNEq:
                resizeReg(instructions[pc].dst);
                if (isInt(registers[instructions[pc].src1]) && isInt(registers[instructions[pc].src2])) {
                    registers[instructions[pc].dst].type = Type::Bool;
                    registers[instructions[pc].dst].b = registers[instructions[pc].src1].i != registers[instructions[pc].src2].i;
                } else {
                    throwError("Incorrect types for CmpNEq");
                }
                pc++;
                break;

            case IR::OP::Type::CmpGt:
                resizeReg(instructions[pc].dst);
                if (isInt(registers[instructions[pc].src1]) && isInt(registers[instructions[pc].src2])) {
                    registers[instructions[pc].dst].type = Type::Bool;
                    registers[instructions[pc].dst].b = registers[instructions[pc].src1].i > registers[instructions[pc].src2].i;
                } else {
                    throwError("Incorrect types for CmpGt");
                }
                pc++;
                break;

            case IR::OP::Type::CmpLs:
                resizeReg(instructions[pc].dst);
                if (isInt(registers[instructions[pc].src1]) && isInt(registers[instructions[pc].src2])) {
                    registers[instructions[pc].dst].type = Type::Bool;
                    registers[instructions[pc].dst].b = registers[instructions[pc].src1].i < registers[instructions[pc].src2].i;
                } else {
                    throwError("Incorrect types for CmpLs");
                }
                pc++;
                break;
            
            case IR::OP::Type::CmpGtEq:
                resizeReg(instructions[pc].dst);
                if (isInt(registers[instructions[pc].src1]) && isInt(registers[instructions[pc].src2])) {
                    registers[instructions[pc].dst].type = Type::Bool;
                    registers[instructions[pc].dst].b = registers[instructions[pc].src1].i >= registers[instructions[pc].src2].i;
                } else {
                    throwError("Incorrect types for CmpGtEq");
                }
                pc++;
                break;
            
            case IR::OP::Type::CmpLsEq:
                resizeReg(instructions[pc].dst);
                if (isInt(registers[instructions[pc].src1]) && isInt(registers[instructions[pc].src2])) {
                    registers[instructions[pc].dst].type = Type::Bool;
                    registers[instructions[pc].dst].b = registers[instructions[pc].src1].i <= registers[instructions[pc].src2].i;
                } else {
                    throwError("Incorrect types for CmpLsEq");
                }
                pc++;
                break;
            
            case IR::OP::Type::Jmp:
                pc = instructions[pc].dst;
                break;
            
            case IR::OP::Type::JmpZ:
                if (isBool(registers[instructions[pc].src1])) {
                    if (registers[instructions[pc].src1].b == false) {
                        pc = instructions[pc].dst;
                    } else {
                        pc++;
                    }
                } else if (isInt(registers[instructions[pc].src1])) {
                    if (registers[instructions[pc].src1].i == 0) {
                        pc = instructions[pc].dst;
                    } else {
                        pc++;
                    }
                } else {
                    throwError("Incorrect type in JmpZ");
                }
                break;
            
            case IR::OP::Type::JmpNZ:
                if (isBool(registers[instructions[pc].src1])) {
                    if (registers[instructions[pc].src1].b == true) {
                        pc = instructions[pc].dst;
                    } else {
                        pc++;
                    }
                } else if (isInt(registers[instructions[pc].src1])) {
                    if (registers[instructions[pc].src1].i != 0) {
                        pc = instructions[pc].dst;
                    } else {
                        pc++;
                    }
                } else {
                    throwError("Incorrect type in JmpNZ");
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

bool VM::isInt(Variable var) {
    if (var.type == Type::Int) return true;
    return false;
}

bool VM::isBool(Variable var) {
    if (var.type == Type::Bool) return true;
    return false;
}