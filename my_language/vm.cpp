#include "vm.h"

void VM::run() {
    while (pc < (*frames.back().instructions).size()) {

        switch (getInstruction(pc).operation) {

            case IR::OP::Type::Int:
                resizeReg(getInstruction(pc).dst);
                getVariable(getInstruction(pc).dst).value = getInstruction(pc).val;
                pc++;
                break;

            case IR::OP::Type::Bool:
                resizeReg(getInstruction(pc).dst);
                getVariable(getInstruction(pc).dst).value = getInstruction(pc).bval;
                pc++;
                break;

            case IR::OP::Type::Add:
                resizeReg(getInstruction(pc).dst);
                if (getVariable(getInstruction(pc).src1).isInt() && getVariable(getInstruction(pc).src2).isInt()) {
                    getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getInt() + getVariable(getInstruction(pc).src2).getInt();
                } else {
                    throwError("Incorrect types for Add");
                }
                pc++;
                break;

            case IR::OP::Type::Sub:
                resizeReg(getInstruction(pc).dst);
                if (getVariable(getInstruction(pc).src1).isInt() && getVariable(getInstruction(pc).src2).isInt()) {
                    getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getInt() - getVariable(getInstruction(pc).src2).getInt();
                } else {
                    throwError("Incorrect types for Sub");
                }
                pc++;
                break;

            case IR::OP::Type::Mul:
                resizeReg(getInstruction(pc).dst);
                if (getVariable(getInstruction(pc).src1).isInt() && getVariable(getInstruction(pc).src2).isInt()) {
                    getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getInt() * getVariable(getInstruction(pc).src2).getInt();
                } else {
                    throwError("Incorrect types for Mul");
                }
                pc++;
                break;

            case IR::OP::Type::Div:
                resizeReg(getInstruction(pc).dst);
                if (getVariable(getInstruction(pc).src1).isInt() && getVariable(getInstruction(pc).src2).isInt()) {
                    if (getVariable(getInstruction(pc).src2).getInt() == 0) throwError("Divide by zero error");
                    getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getInt() / getVariable(getInstruction(pc).src2).getInt();
                } else {
                    throwError("Incorrect types for Div");
                }
                pc++;
                break;

            case IR::OP::Type::Neg:
                resizeReg(getInstruction(pc).dst);
                if (getVariable(getInstruction(pc).src1).isInt()) {
                    getVariable(getInstruction(pc).dst).value = -getVariable(getInstruction(pc).src1).getInt();
                } else {
                    throwError("Incorrect types for Neg");
                }
                pc++;
                break;

            case IR::OP::Type::Assign: {
                frames.back().varMap[getInstruction(pc).src1] = getVariable(getInstruction(pc).dst);
                pc++;
                break;
            }

            case IR::OP::Type::Load: {
                resizeReg(getInstruction(pc).dst);
                getVariable(getInstruction(pc).dst) = frames.back().varMap[getInstruction(pc).src1];
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

                newFrame.varMap.resize(funMeta.varCount);

                frames.push_back(newFrame);

                for (int i = 0; i < funMeta.argsCount; i++) {
                    frames.back().varMap[i] = registers[frames.back().bottomStack + i];
                }

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
                if (getVariable(getInstruction(pc).src1).isBool()) {
                    if (std::get<bool>(getVariable(getInstruction(pc).src1).value) == false) {
                        pc = getInstruction(pc).dst;
                    } else {
                        pc++;
                    }
                } else if (getVariable(getInstruction(pc).src1).isInt()) {
                    if (std::get<int>(getVariable(getInstruction(pc).src1).value) == 0) {
                        pc = getInstruction(pc).dst;
                    } else {
                        pc++;
                    }
                } else {
                    throwError("Incorrect type in JmpZ");
                }
                break;
            
            case IR::OP::Type::JmpNZ:
                if (getVariable(getInstruction(pc).src1).isBool()) {
                    if (std::get<bool>(getVariable(getInstruction(pc).src1).value) == true) {
                        pc = getInstruction(pc).dst;
                    } else {
                        pc++;
                    }
                } else if (getVariable(getInstruction(pc).src1).isInt()) {
                    if (std::get<int>(getVariable(getInstruction(pc).src1).value) != 0) {
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

void VM::runCmp() {
    if (getVariable(getInstruction(pc).src1).isInt() && getVariable(getInstruction(pc).src2).isInt()) {
        switch (getInstruction(pc).operation) {
            
            case IR::OP::Type::CmpEq:
                getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getInt() == getVariable(getInstruction(pc).src2).getInt();
                break;

            case IR::OP::Type::CmpNEq:
                getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getInt() != getVariable(getInstruction(pc).src2).getInt();
                break;

            case IR::OP::Type::CmpGt:
                getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getInt() > getVariable(getInstruction(pc).src2).getInt();
                break;

            case IR::OP::Type::CmpLs:
                getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getInt() < getVariable(getInstruction(pc).src2).getInt();
                break;
            
            case IR::OP::Type::CmpGtEq:
                getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getInt() >= getVariable(getInstruction(pc).src2).getInt();
                break;
            
            case IR::OP::Type::CmpLsEq:
                getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getInt() <= getVariable(getInstruction(pc).src2).getInt();
                break;

            default:
                throwError("Incorrect types for Cmp");
        }
    } else if (getVariable(getInstruction(pc).src1).isBool() && getVariable(getInstruction(pc).src2).isBool()) {
        switch (getInstruction(pc).operation) {
            
            case IR::OP::Type::CmpEq:
                getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getBool() == getVariable(getInstruction(pc).src2).getBool();
                break;

            case IR::OP::Type::CmpNEq:
                getVariable(getInstruction(pc).dst).value = getVariable(getInstruction(pc).src1).getBool() != getVariable(getInstruction(pc).src2).getBool();
                break;

            default:
                throwError("Incorrect types for Cmp");
        }
    } else {
        throwError("Incorrect types for Cmp");
    }
}