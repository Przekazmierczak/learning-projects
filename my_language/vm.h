#ifndef VM_H
#define VM_H

#include <iostream>
#include <vector>
#include <unordered_map>

#include "ir.h"
#include "helper.h"
#include "variable.h"
#include "stdlib.h"

struct VM {
    struct Frame {
        std::vector<IR::OP>* instructions;
        int returnPC;
        int returnReg;
        int bottomStack;
        int topStack;

        std::vector<Variable> varMap;
    };

    std::vector<IR::OP>& mainInstructions;
    std::vector<IR::OP>& functionsInstructions;
    std::vector<IR::FunctionMeta>& functionsMap;

    std::vector<Variable> registers;
    std::vector<Frame> frames;

    int pc = 0;
    int currBottomStack;
    int currTopStack;
    int registersEnd;
    
    using Funct = void(VM::*)(IR::OP& instruction);
    static Funct dispatch[];

    VM(
        std::vector<IR::OP>& newMainInstructions,
        std::vector<IR::OP>& newFunctionsInstructions,
        std::vector<IR::FunctionMeta>& newFunctionsMap
    ) : 
        mainInstructions(newMainInstructions),
        functionsInstructions(newFunctionsInstructions),
        functionsMap(newFunctionsMap)
    {
        Frame mainFrame;
        mainFrame.instructions = &mainInstructions;
        mainFrame.returnPC = -1;
        mainFrame.bottomStack = 0;
        mainFrame.topStack = functionsMap[0].argsCount + functionsMap[0].regCount;
        mainFrame.varMap.resize(functionsMap[0].varCount);

        frames.push_back(mainFrame);
        registersEnd = mainFrame.topStack;
        resizeReg(mainFrame.topStack - 1);

        run();
    }

    void Int(IR::OP& inst); void Bool(IR::OP& inst);
    void Add(IR::OP& inst); void Sub(IR::OP& inst); void Mul(IR::OP& inst); void Div(IR::OP& inst);
    void Neg(IR::OP& inst);
    void Assign(IR::OP& inst); void Load(IR::OP& inst);
    void CmpEq(IR::OP& inst); void CmpNEq(IR::OP& inst); void CmpGt(IR::OP& inst);
    void CmpLs(IR::OP& inst); void CmpGtEq(IR::OP& inst); void CmpLsEq(IR::OP& inst);
    void Jmp(IR::OP& inst); void JmpZ(IR::OP& inst); void JmpNZ(IR::OP& inst);
    void Call(IR::OP& inst); void Ret(IR::OP& inst);
    void Push(IR::OP& inst);

    void run();
    void resizeReg(int dst);

    void pushFrame(Frame newFrame);
    void popFrame();

    const IR::OP& getInstruction(int pc);
    Variable& getVariable(int offset);

    void runCmp(IR::OP& inst);
};

#endif