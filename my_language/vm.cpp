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

            case IR::OP::Type::Bool:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst].type = VM::Type::Bool;
                registers[instructions[pc].dst].b = instructions[pc].bval;
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
                if (findInMap(maps.size() - 1, instructions[pc].name)) {
                    throwError("Variable \"" + instructions[pc].name + "\" is already declared");
                } else {
                    maps[maps.size() - 1][instructions[pc].name] = -1;
                }
                pc++;
                break;

            case IR::OP::Type::Assign: {
                int index = findInMaps(instructions[pc].name);
                if (index == -1) {
                    throwError("Variable \"" + instructions[pc].name + "\" was never declared");
                }
                maps[index][instructions[pc].name] = instructions[pc].dst;
                pc++;
                break;
            }

            case IR::OP::Type::Load: {
                int index = findInMaps(instructions[pc].name);
                if (index == -1) {
                    throwError("Unknown variable name: \"" + instructions[pc].name + "\"");
                }
                if (maps[index][instructions[pc].name] == -1) {
                    throwError("Variable \"" + instructions[pc].name + "\" was never initialized");
                }
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = registers[maps[index][instructions[pc].name]];
                pc++;
                break;
            }

            case IR::OP::Type::Block:
                maps.emplace_back();
                pc++;
                break;

            case IR::OP::Type::Deblock:
                maps.pop_back();
                pc++;
                break;

            case IR::OP::Type::CmpEq:
            case IR::OP::Type::CmpNEq:
            case IR::OP::Type::CmpGt:
            case IR::OP::Type::CmpLs:
            case IR::OP::Type::CmpGtEq:
            case IR::OP::Type::CmpLsEq:
                resizeReg(instructions[pc].dst);
                runCmp(instructions);
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

bool VM::findInMap(int index, const std::string& name) {
    return maps[index].find(name) != maps[index].end();
}

int VM::findInMaps(const std::string& name) {
    for (int i = maps.size() - 1; i >= 0; i--) {
        if (findInMap(i, name)) return i;
    }
    return -1;
}

void VM::runCmp(const std::vector<IR::OP>& instructions) {
    if (isInt(registers[instructions[pc].src1]) && isInt(registers[instructions[pc].src2])) {
        registers[instructions[pc].dst].type = Type::Bool;

        switch (instructions[pc].operation) {
            
            case IR::OP::Type::CmpEq:
                registers[instructions[pc].dst].b = registers[instructions[pc].src1].i == registers[instructions[pc].src2].i;
                break;

            case IR::OP::Type::CmpNEq:
                registers[instructions[pc].dst].b = registers[instructions[pc].src1].i != registers[instructions[pc].src2].i;
                break;

            case IR::OP::Type::CmpGt:
                registers[instructions[pc].dst].b = registers[instructions[pc].src1].i > registers[instructions[pc].src2].i;
                break;

            case IR::OP::Type::CmpLs:
                registers[instructions[pc].dst].b = registers[instructions[pc].src1].i < registers[instructions[pc].src2].i;
                break;
            
            case IR::OP::Type::CmpGtEq:
                registers[instructions[pc].dst].b = registers[instructions[pc].src1].i >= registers[instructions[pc].src2].i;
                break;
            
            case IR::OP::Type::CmpLsEq:
                registers[instructions[pc].dst].b = registers[instructions[pc].src1].i <= registers[instructions[pc].src2].i;
                break;

            default:
                throwError("Incorrect types for Cmp");
        }
    } else {
        throwError("Incorrect types for Cmp");
    }
}