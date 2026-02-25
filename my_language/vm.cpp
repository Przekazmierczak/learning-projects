#include "vm.h"

void VM::run() {
    while (pc < instructions.size()) {
        switch (getInstruction(pc).operation) {

            case IR::OP::Type::Int:
                resizeReg(getInstruction(pc).dst);
                registers[getInstruction(pc).dst].type = VM::Type::Int;
                registers[getInstruction(pc).dst].i = getInstruction(pc).val;
                pc++;
                break;

            case IR::OP::Type::Bool:
                resizeReg(getInstruction(pc).dst);
                registers[getInstruction(pc).dst].type = VM::Type::Bool;
                registers[getInstruction(pc).dst].b = getInstruction(pc).bval;
                pc++;
                break;

            case IR::OP::Type::Add:
                resizeReg(getInstruction(pc).dst);
                if (isInt(registers[getInstruction(pc).src1]) && isInt(registers[getInstruction(pc).src2])) {
                    registers[getInstruction(pc).dst].type = Type::Int;
                    registers[getInstruction(pc).dst].i = registers[getInstruction(pc).src1].i + registers[getInstruction(pc).src2].i;
                } else {
                    throwError("Incorrect types for Add");
                }
                pc++;
                break;

            case IR::OP::Type::Sub:
                resizeReg(getInstruction(pc).dst);
                if (isInt(registers[getInstruction(pc).src1]) && isInt(registers[getInstruction(pc).src2])) {
                    registers[getInstruction(pc).dst].type = Type::Int;
                    registers[getInstruction(pc).dst].i = registers[getInstruction(pc).src1].i - registers[getInstruction(pc).src2].i;
                } else {
                    throwError("Incorrect types for Sub");
                }
                pc++;
                break;

            case IR::OP::Type::Mul:
                resizeReg(getInstruction(pc).dst);
                if (isInt(registers[getInstruction(pc).src1]) && isInt(registers[getInstruction(pc).src2])) {
                    registers[getInstruction(pc).dst].type = Type::Int;
                    registers[getInstruction(pc).dst].i = registers[getInstruction(pc).src1].i * registers[getInstruction(pc).src2].i;
                } else {
                    throwError("Incorrect types for Mul");
                }
                pc++;
                break;

            case IR::OP::Type::Div:
                resizeReg(getInstruction(pc).dst);
                if (isInt(registers[getInstruction(pc).src1]) && isInt(registers[getInstruction(pc).src2])) {
                    if (registers[getInstruction(pc).src2].i == 0) throwError("Divide by zero error");
                    registers[getInstruction(pc).dst].type = Type::Int;
                    registers[getInstruction(pc).dst].i = registers[getInstruction(pc).src1].i / registers[getInstruction(pc).src2].i;
                } else {
                    throwError("Incorrect types for Div");
                }
                pc++;
                break;

            case IR::OP::Type::Neg:
                resizeReg(getInstruction(pc).dst);
                if (isInt(registers[getInstruction(pc).src1])) {
                    registers[getInstruction(pc).dst].type = Type::Int;
                    registers[getInstruction(pc).dst].i = -registers[getInstruction(pc).src1].i;
                } else {
                    throwError("Incorrect types for Div");
                }
                pc++;
                break;

            case IR::OP::Type::Dec:
                if (findInMap(maps.size() - 1, getInstruction(pc).name)) {
                    throwError("Variable \"" + getInstruction(pc).name + "\" is already declared");
                } else {
                    maps[maps.size() - 1][getInstruction(pc).name] = -1;
                }
                pc++;
                break;

            case IR::OP::Type::Assign: {
                int index = findInMaps(getInstruction(pc).name);
                if (index == -1) {
                    throwError("Variable \"" + getInstruction(pc).name + "\" was never declared");
                }
                maps[index][getInstruction(pc).name] = getInstruction(pc).dst;
                pc++;
                break;
            }

            case IR::OP::Type::Load: {
                int index = findInMaps(getInstruction(pc).name);
                if (index == -1) {
                    throwError("Unknown variable name: \"" + getInstruction(pc).name + "\"");
                }
                if (maps[index][getInstruction(pc).name] == -1) {
                    throwError("Variable \"" + getInstruction(pc).name + "\" was never initialized");
                }
                resizeReg(getInstruction(pc).dst);
                registers[getInstruction(pc).dst] = registers[maps[index][getInstruction(pc).name]];
                pc++;
                break;
            }

            case IR::OP::Type::Call: {
                IR::OP caller = getInstruction(pc);
                IR::FunctionMeta funMeta = functionsMap.at(caller.name);

                Frame newFrame;
                newFrame.returnPC = pc + 1;
                newFrame.returnReg = caller.dst;
                newFrame.bottomStack = frames.front().topStack;
                newFrame.topStack = funMeta.argsCount + funMeta.regCount;

                resizeReg(newFrame.topStack);
                registersEnd = newFrame.topStack;

                frames.push_back(newFrame);
                pc = funMeta.startPC;
                break;
            }

            case IR::OP::Type::Ret: {
                Variable result = registers[getInstruction(pc).dst];
                int returnReg = frames.front().returnReg;
                int returnPc = frames.front().returnPC;

                frames.pop_back();
                registers[returnReg] = result;

                pc = returnPc;
                registersEnd = frames.front().topStack;
                break;
            }

            case IR::OP::Type::Push: {
                resizeReg(registersEnd);
                registers[registersEnd] = getInstruction(pc).src1;
                pc++;
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
                resizeReg(getInstruction(pc).dst);
                runCmp();
                pc++;
                break;
            
            case IR::OP::Type::Jmp:
                pc = getInstruction(pc).dst;
                break;
            
            case IR::OP::Type::JmpZ:
                if (isBool(registers[getInstruction(pc).src1])) {
                    if (registers[getInstruction(pc).src1].b == false) {
                        pc = getInstruction(pc).dst;
                    } else {
                        pc++;
                    }
                } else if (isInt(registers[getInstruction(pc).src1])) {
                    if (registers[getInstruction(pc).src1].i == 0) {
                        pc = getInstruction(pc).dst;
                    } else {
                        pc++;
                    }
                } else {
                    throwError("Incorrect type in JmpZ");
                }
                break;
            
            case IR::OP::Type::JmpNZ:
                if (isBool(registers[getInstruction(pc).src1])) {
                    if (registers[getInstruction(pc).src1].b == true) {
                        pc = getInstruction(pc).dst;
                    } else {
                        pc++;
                    }
                } else if (isInt(registers[getInstruction(pc).src1])) {
                    if (registers[getInstruction(pc).src1].i != 0) {
                        pc = getInstruction(pc).dst;
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

void VM::pushFrame(Frame newFrame) {
    if (frames.size() > 0) {
        
    } else {

    }
    frames.push_back(newFrame);
}

void VM::popFrame() {
    if (frames.size() > 0) {
        pc = frames.front().returnPC;
        frames.pop_back();
        currTopStack = frames.front().topStack;
        currBottomStack = frames.front().bottomStack;
    } else {
        throwError("__main__ frame should not be poped");
    }
}

const IR::OP& VM::getInstruction(int pc) {
    if (frames.size() > 1) {
        return functionsInstructions[pc];
    }
    return instructions[pc];
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

void VM::runCmp() {
    if (isInt(registers[getInstruction(pc).src1]) && isInt(registers[getInstruction(pc).src2])) {
        registers[getInstruction(pc).dst].type = Type::Bool;

        switch (getInstruction(pc).operation) {
            
            case IR::OP::Type::CmpEq:
                registers[getInstruction(pc).dst].b = registers[getInstruction(pc).src1].i == registers[getInstruction(pc).src2].i;
                break;

            case IR::OP::Type::CmpNEq:
                registers[getInstruction(pc).dst].b = registers[getInstruction(pc).src1].i != registers[getInstruction(pc).src2].i;
                break;

            case IR::OP::Type::CmpGt:
                registers[getInstruction(pc).dst].b = registers[getInstruction(pc).src1].i > registers[getInstruction(pc).src2].i;
                break;

            case IR::OP::Type::CmpLs:
                registers[getInstruction(pc).dst].b = registers[getInstruction(pc).src1].i < registers[getInstruction(pc).src2].i;
                break;
            
            case IR::OP::Type::CmpGtEq:
                registers[getInstruction(pc).dst].b = registers[getInstruction(pc).src1].i >= registers[getInstruction(pc).src2].i;
                break;
            
            case IR::OP::Type::CmpLsEq:
                registers[getInstruction(pc).dst].b = registers[getInstruction(pc).src1].i <= registers[getInstruction(pc).src2].i;
                break;

            default:
                throwError("Incorrect types for Cmp");
        }
    } else if (isBool(registers[getInstruction(pc).src1]) && isBool(registers[getInstruction(pc).src2])) {
        registers[getInstruction(pc).dst].type = Type::Bool;

        switch (getInstruction(pc).operation) {
            
            case IR::OP::Type::CmpEq:
                registers[getInstruction(pc).dst].b = registers[getInstruction(pc).src1].b == registers[getInstruction(pc).src2].b;
                break;

            case IR::OP::Type::CmpNEq:
                registers[getInstruction(pc).dst].b = registers[getInstruction(pc).src1].b != registers[getInstruction(pc).src2].b;
                break;

            default:
                throwError("Incorrect types for Cmp");
        }
    } else {
        throwError("Incorrect types for Cmp");
    }
}