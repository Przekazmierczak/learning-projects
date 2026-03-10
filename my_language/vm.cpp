#include "vm.h"

void VM::run() {
    while (pc < (*frames.back().instructions).size()) {
        IR::OP inst = getInstruction(pc);
        (this->*dispatch[inst.operation])(inst);
    }

    if (frames.size() > 1) {
        throwError("Program termiante from function");
    }
}

VM::Funct VM::dispatch[] = {
    &VM::Int, &VM::Bool,
    &VM::Add, &VM::Sub, &VM::Mul, &VM::Div,
    &VM::Neg,
    &VM::Assign, &VM::Load,
    &VM::CmpEq, &VM::CmpNEq, &VM::CmpGt, &VM::CmpLs, &VM::CmpGtEq, &VM::CmpLsEq,
    &VM::Jmp, &VM::JmpZ, &VM::JmpNZ,
    &VM::Call, &VM::Ret,
    &VM::Push
};

void VM::Int(IR::OP& inst) {
    getVariable(inst.dst).value = inst.val;
    pc++;
}

void VM::Bool(IR::OP& inst) {
    getVariable(inst.dst).value = inst.bval;
    pc++;
}

void VM::Add(IR::OP& inst) {
    auto& src1 = getVariable(inst.src1);
    auto& src2 = getVariable(inst.src2);

    if (src1.isInt() && src2.isInt()) {
        getVariable(inst.dst).value = src1.getInt() + src2.getInt();
    } else {
        throwError("Incorrect types for Add");
    }
    pc++;
}

void VM::Sub(IR::OP& inst) {
    auto& src1 = getVariable(inst.src1);
    auto& src2 = getVariable(inst.src2);

    if (src1.isInt() && src2.isInt()) {
        getVariable(inst.dst).value = src1.getInt() - src2.getInt();
    } else {
        throwError("Incorrect types for Sub");
    }
    pc++;
}

void VM::Mul(IR::OP& inst) {
    auto& src1 = getVariable(inst.src1);
    auto& src2 = getVariable(inst.src2);

    if (src1.isInt() && src2.isInt()) {
        getVariable(inst.dst).value = src1.getInt() * src2.getInt();
    } else {
        throwError("Incorrect types for Mul");
    }
    pc++;
}

void VM::Div(IR::OP& inst) {
    auto& src1 = getVariable(inst.src1);
    auto& src2 = getVariable(inst.src2);

    if (src1.isInt() && src2.isInt()) {
        if (src2.getInt() == 0) throwError("Divide by zero error");
        getVariable(inst.dst).value = src1.getInt() / src2.getInt();
    } else {
        throwError("Incorrect types for Div");
    }
    pc++;
}

void VM::Neg(IR::OP& inst) {
    auto& src1 = getVariable(inst.src1);

    if (src1.isInt()) {
        getVariable(inst.dst).value = -src1.getInt();
    } else {
        throwError("Incorrect types for Neg");
    }
    pc++;
}

void VM::Assign(IR::OP& inst) {
    frames.back().varMap[inst.src1] = getVariable(inst.dst);
    pc++;
}

void VM::Load(IR::OP& inst) {
    getVariable(inst.dst) = frames.back().varMap[inst.src1];
    pc++;
}

void VM::Call(IR::OP& inst) {
    FunctionMeta funMeta = functionsMap[inst.src1];
    // check
    if (funMeta.native) {
        std::vector<Variable> args;
        for (int i = 0; i < funMeta.argsCount; i++) {
            args.push_back(registers[frames.back().topStack + i]);
        }
        getVariable(inst.dst) = nativeFunctions[funMeta.startPC](args);

        registersEnd = frames.back().topStack;

        pc++;
    } else {
        Frame newFrame;
        newFrame.instructions = &functionsInstructions;
        newFrame.returnPC = pc + 1;
        newFrame.returnReg = inst.dst;
        newFrame.bottomStack = frames.back().topStack;
        newFrame.topStack = funMeta.argsCount + funMeta.regCount + newFrame.bottomStack;
    
        resizeReg(newFrame.topStack - 1);
        registersEnd = newFrame.topStack;
    
        newFrame.varMap.resize(funMeta.varCount);
    
        frames.push_back(newFrame);
    
        for (int i = 0; i < funMeta.argsCount; i++) {
            frames.back().varMap[i] = registers[frames.back().bottomStack + i];
        }
    
        pc = funMeta.startPC;
    }
}

void VM::Ret(IR::OP& inst) {
    Variable result = getVariable(inst.dst);
    int returnReg = frames.back().returnReg;
    int returnPc = frames.back().returnPC;

    frames.pop_back();
    getVariable(returnReg) = result;

    pc = returnPc;
    registersEnd = frames.back().topStack;
}

void VM::Push(IR::OP& inst) {
    resizeReg(registersEnd);
    registers[registersEnd] = getVariable(inst.src1);
    registersEnd++;
    pc++;
}

void VM::CmpEq(IR::OP& inst) {
    runCmp(inst);
    pc++;
}

void VM::CmpNEq(IR::OP& inst) {
    runCmp(inst);
    pc++;
}

void VM::CmpGt(IR::OP& inst) {
    runCmp(inst);
    pc++;
}

void VM::CmpLs(IR::OP& inst) {
    runCmp(inst);
    pc++;
}

void VM::CmpGtEq(IR::OP& inst) {
    runCmp(inst);
    pc++;
}

void VM::CmpLsEq(IR::OP& inst) {
    runCmp(inst);
    pc++;
}

void VM::Jmp(IR::OP& inst) {
    pc = inst.dst;
}

void VM::JmpZ(IR::OP& inst) {
    auto& src1 = getVariable(inst.src1);

    if (src1.isBool()) {
        if (std::get<bool>(src1.value) == false) {
            pc = inst.dst;
        } else {
            pc++;
        }
    } else if (src1.isInt()) {
        if (std::get<int>(src1.value) == 0) {
            pc = inst.dst;
        } else {
            pc++;
        }
    } else {
        throwError("Incorrect type in JmpZ");
    }
}

void VM::JmpNZ(IR::OP& inst) {
    auto& src1 = getVariable(inst.src1);

    if (src1.isBool()) {
        if (std::get<bool>(src1.value) == true) {
            pc = inst.dst;
        } else {
            pc++;
        }
    } else if (src1.isInt()) {
        if (std::get<int>(src1.value) != 0) {
            pc = inst.dst;
        } else {
            pc++;
        }
    } else {
        throwError("Incorrect type in JmpNZ");
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

Variable& VM::getVariable(int offset) {
    int index = frames.back().bottomStack + offset;
    if (index >= registers.size()) {
        throwError("Index out of registers size in getVariable");
    }
    return registers[index];
}

void VM::runCmp(IR::OP& inst) {
    auto& src1 = getVariable(inst.src1);
    auto& src2 = getVariable(inst.src2);

    if (src1.isInt() && getVariable(inst.src2).isInt()) {
        switch (inst.operation) {
            
            case IR::OP::Type::CmpEq:
                getVariable(inst.dst).value = src1.getInt() == src2.getInt();
                break;

            case IR::OP::Type::CmpNEq:
                getVariable(inst.dst).value = src1.getInt() != src2.getInt();
                break;

            case IR::OP::Type::CmpGt:
                getVariable(inst.dst).value = src1.getInt() > src2.getInt();
                break;

            case IR::OP::Type::CmpLs:
                getVariable(inst.dst).value = src1.getInt() < src2.getInt();
                break;
            
            case IR::OP::Type::CmpGtEq:
                getVariable(inst.dst).value = src1.getInt() >= src2.getInt();
                break;
            
            case IR::OP::Type::CmpLsEq:
                getVariable(inst.dst).value = src1.getInt() <= src2.getInt();
                break;

            default:
                throwError("Incorrect types for Cmp");
        }
    } else if (src1.isBool() && src2.isBool()) {
        switch (inst.operation) {
            
            case IR::OP::Type::CmpEq:
                getVariable(inst.dst).value = src1.getBool() == src2.getBool();
                break;

            case IR::OP::Type::CmpNEq:
                getVariable(inst.dst).value = src1.getBool() != src2.getBool();
                break;

            default:
                throwError("Incorrect types for Cmp");
        }
    } else {
        throwError("Incorrect types for Cmp");
    }
}