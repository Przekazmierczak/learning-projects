#include "vm.h"

void VM::run() {
    while (pc < (*frames.back().instructions).size()) {

        switch (getInstruction(pc).operation) {

            case IR::OP::Type::Int:
                resizeReg(getInstruction(pc).dst);
                getVariable(getInstruction(pc).dst).type = VM::Type::Int;
                getVariable(getInstruction(pc).dst).i = getInstruction(pc).val;
                pc++;
                break;

            case IR::OP::Type::Bool:
                resizeReg(getInstruction(pc).dst);
                getVariable(getInstruction(pc).dst).type = VM::Type::Bool;
                getVariable(getInstruction(pc).dst).b = getInstruction(pc).bval;
                pc++;
                break;

            case IR::OP::Type::Add:
                resizeReg(getInstruction(pc).dst);
                if (isInt(getVariable(getInstruction(pc).src1)) && isInt(getVariable(getInstruction(pc).src2))) {
                    getVariable(getInstruction(pc).dst).type = Type::Int;
                    getVariable(getInstruction(pc).dst).i = getVariable(getInstruction(pc).src1).i + getVariable(getInstruction(pc).src2).i;
                } else {
                    throwError("Incorrect types for Add");
                }
                pc++;
                break;

            case IR::OP::Type::Sub:
                resizeReg(getInstruction(pc).dst);
                if (isInt(getVariable(getInstruction(pc).src1)) && isInt(getVariable(getInstruction(pc).src2))) {
                    getVariable(getInstruction(pc).dst).type = Type::Int;
                    getVariable(getInstruction(pc).dst).i = getVariable(getInstruction(pc).src1).i - getVariable(getInstruction(pc).src2).i;
                } else {
                    throwError("Incorrect types for Sub");
                }
                pc++;
                break;

            case IR::OP::Type::Mul:
                resizeReg(getInstruction(pc).dst);
                if (isInt(getVariable(getInstruction(pc).src1)) && isInt(getVariable(getInstruction(pc).src2))) {
                    getVariable(getInstruction(pc).dst).type = Type::Int;
                    getVariable(getInstruction(pc).dst).i = getVariable(getInstruction(pc).src1).i * getVariable(getInstruction(pc).src2).i;
                } else {
                    throwError("Incorrect types for Mul");
                }
                pc++;
                break;

            case IR::OP::Type::Div:
                resizeReg(getInstruction(pc).dst);
                if (isInt(getVariable(getInstruction(pc).src1)) && isInt(getVariable(getInstruction(pc).src2))) {
                    if (getVariable(getInstruction(pc).src2).i == 0) throwError("Divide by zero error");
                    getVariable(getInstruction(pc).dst).type = Type::Int;
                    getVariable(getInstruction(pc).dst).i = getVariable(getInstruction(pc).src1).i / getVariable(getInstruction(pc).src2).i;
                } else {
                    throwError("Incorrect types for Div");
                }
                pc++;
                break;

            case IR::OP::Type::Neg:
                resizeReg(getInstruction(pc).dst);
                if (isInt(getVariable(getInstruction(pc).src1))) {
                    getVariable(getInstruction(pc).dst).type = Type::Int;
                    getVariable(getInstruction(pc).dst).i = -getVariable(getInstruction(pc).src1).i;
                } else {
                    throwError("Incorrect types for Div");
                }
                pc++;
                break;

            case IR::OP::Type::Dec:
                if (findInMap(frames.back().maps.size() - 1, getInstruction(pc).name)) {
                    throwError("Variable \"" + getInstruction(pc).name + "\" is already declared");
                } else {
                    frames.back().maps[frames.back().maps.size() - 1][getInstruction(pc).name] = -1;
                }
                pc++;
                break;

            case IR::OP::Type::Assign: {
                int index = findInMaps(getInstruction(pc).name);
                if (index == -1) {
                    throwError("Variable \"" + getInstruction(pc).name + "\" was never declared");
                }
                frames.back().maps[index][getInstruction(pc).name] = getVariable(getInstruction(pc).dst);
                pc++;
                break;
            }

            case IR::OP::Type::Load: {
                int index = findInMaps(getInstruction(pc).name);
                if (index == -1) {
                    throwError("Unknown variable name: \"" + getInstruction(pc).name + "\"");
                }
                if (frames.back().maps[index][getInstruction(pc).name] == -1) {
                    throwError("Variable \"" + getInstruction(pc).name + "\" was never initialized");
                }
                resizeReg(getInstruction(pc).dst);
                getVariable(getInstruction(pc).dst) = frames.back().maps[index][getInstruction(pc).name];
                pc++;
                break;
            }

            case IR::OP::Type::Call: {
                IR::OP caller = getInstruction(pc);
                IR::FunctionMeta funMeta = functionsMap.at(caller.name);

                Frame newFrame;
                newFrame.instructions = &functionsInstructions;
                newFrame.returnPC = pc + 1;
                newFrame.returnReg = caller.dst;
                newFrame.bottomStack = frames.back().topStack;
                newFrame.topStack = funMeta.argsCount + funMeta.regCount + newFrame.bottomStack;

                resizeReg(newFrame.topStack);
                registersEnd = newFrame.topStack;
                newFrame.maps.emplace_back();

                frames.push_back(newFrame);

                pc = funMeta.startPC;
                break;
            }

            case IR::OP::Type::Ret: {
                Variable result = getVariable(getInstruction(pc).dst);
                int returnReg = frames.back().returnReg;
                int returnPc = frames.back().returnPC;

                frames.pop_back();
                getVariable(returnReg) = result;

                pc = returnPc;
                registersEnd = frames.back().topStack;
                break;
            }

            case IR::OP::Type::Push: {
                resizeReg(registersEnd);
                registers[registersEnd] = getVariable(getInstruction(pc).src1);
                registersEnd++;
                pc++;
                break;
            }

            case IR::OP::Type::Block:
                frames.back().maps.emplace_back();
                pc++;
                break;

            case IR::OP::Type::Deblock:
                frames.back().maps.pop_back();
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
                if (isBool(getVariable(getInstruction(pc).src1))) {
                    if (getVariable(getInstruction(pc).src1).b == false) {
                        pc = getInstruction(pc).dst;
                    } else {
                        pc++;
                    }
                } else if (isInt(getVariable(getInstruction(pc).src1))) {
                    if (getVariable(getInstruction(pc).src1).i == 0) {
                        pc = getInstruction(pc).dst;
                    } else {
                        pc++;
                    }
                } else {
                    throwError("Incorrect type in JmpZ");
                }
                break;
            
            case IR::OP::Type::JmpNZ:
                if (isBool(getVariable(getInstruction(pc).src1))) {
                    if (getVariable(getInstruction(pc).src1).b == true) {
                        pc = getInstruction(pc).dst;
                    } else {
                        pc++;
                    }
                } else if (isInt(getVariable(getInstruction(pc).src1))) {
                    if (getVariable(getInstruction(pc).src1).i != 0) {
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

    if (frames.size() > 1) {
        throwError("Program termiante from function");
    }
}

void VM::resizeReg(int dst) {
    if (dst >= registers.size()) {
        registers.resize(dst + 1);
    }
}

const IR::OP& VM::getInstruction(int pc) {
    return (*frames.back().instructions)[pc];
}

VM::Variable& VM::getVariable(int offset) {
    return registers[frames.back().bottomStack + offset];
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
    return frames.back().maps[index].find(name) != frames.back().maps[index].end();
}

int VM::findInMaps(const std::string& name) {
    for (int i = frames.back().maps.size() - 1; i >= 0; i--) {
        if (findInMap(i, name)) return i;
    }
    return -1;
}

void VM::runCmp() {
    if (isInt(getVariable(getInstruction(pc).src1)) && isInt(getVariable(getInstruction(pc).src2))) {
        getVariable(getInstruction(pc).dst).type = Type::Bool;

        switch (getInstruction(pc).operation) {
            
            case IR::OP::Type::CmpEq:
                getVariable(getInstruction(pc).dst).b = getVariable(getInstruction(pc).src1).i == getVariable(getInstruction(pc).src2).i;
                break;

            case IR::OP::Type::CmpNEq:
                getVariable(getInstruction(pc).dst).b = getVariable(getInstruction(pc).src1).i != getVariable(getInstruction(pc).src2).i;
                break;

            case IR::OP::Type::CmpGt:
                getVariable(getInstruction(pc).dst).b = getVariable(getInstruction(pc).src1).i > getVariable(getInstruction(pc).src2).i;
                break;

            case IR::OP::Type::CmpLs:
                getVariable(getInstruction(pc).dst).b = getVariable(getInstruction(pc).src1).i < getVariable(getInstruction(pc).src2).i;
                break;
            
            case IR::OP::Type::CmpGtEq:
                getVariable(getInstruction(pc).dst).b = getVariable(getInstruction(pc).src1).i >= getVariable(getInstruction(pc).src2).i;
                break;
            
            case IR::OP::Type::CmpLsEq:
                getVariable(getInstruction(pc).dst).b = getVariable(getInstruction(pc).src1).i <= getVariable(getInstruction(pc).src2).i;
                break;

            default:
                throwError("Incorrect types for Cmp");
        }
    } else if (isBool(getVariable(getInstruction(pc).src1)) && isBool(getVariable(getInstruction(pc).src2))) {
        getVariable(getInstruction(pc).dst).type = Type::Bool;

        switch (getInstruction(pc).operation) {
            
            case IR::OP::Type::CmpEq:
                getVariable(getInstruction(pc).dst).b = getVariable(getInstruction(pc).src1).b == getVariable(getInstruction(pc).src2).b;
                break;

            case IR::OP::Type::CmpNEq:
                getVariable(getInstruction(pc).dst).b = getVariable(getInstruction(pc).src1).b != getVariable(getInstruction(pc).src2).b;
                break;

            default:
                throwError("Incorrect types for Cmp");
        }
    } else {
        throwError("Incorrect types for Cmp");
    }
}