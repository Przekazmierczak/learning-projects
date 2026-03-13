#ifndef VM_H
#define VM_H

#include <iostream>
#include <vector>
#include <unordered_map>

#include "ir.h"
#include "helper.h"
#include "value.h"

struct VM {
    struct Frame {
        std::vector<IR::Opcode>* instructions;
        int returnPC;
        int returnReg;
        int bottomStack;
        int topStack;

        std::vector<Value> varMap;
    };

    std::vector<IR::Opcode>& mainInstructions;
    std::vector<IR::Opcode>& functionsInstructions;
    std::vector<IR::FunctionMeta>& functionsMap;

    std::vector<Value> registers;
    std::vector<Frame> frames;

    int pc = 0;
    int currBottomStack;
    int currTopStack;
    int registersEnd;
    
    using Funct = void(VM::*)(IR::Opcode& instruction);
    static Funct dispatch[];

    VM(
        std::vector<IR::Opcode>& newMainInstructions,
        std::vector<IR::Opcode>& newFunctionsInstructions,
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

    void Int(IR::Opcode& inst); void Bool(IR::Opcode& inst); void String(IR::Opcode& inst);
    void Add(IR::Opcode& inst); void Sub(IR::Opcode& inst); void Mul(IR::Opcode& inst); void Div(IR::Opcode& inst);
    void Neg(IR::Opcode& inst);
    void Assign(IR::Opcode& inst); void Load(IR::Opcode& inst);
    void CmpEq(IR::Opcode& inst); void CmpNEq(IR::Opcode& inst); void CmpGt(IR::Opcode& inst);
    void CmpLs(IR::Opcode& inst); void CmpGtEq(IR::Opcode& inst); void CmpLsEq(IR::Opcode& inst);
    void Jmp(IR::Opcode& inst); void JmpZ(IR::Opcode& inst); void JmpNZ(IR::Opcode& inst);
    void Call(IR::Opcode& inst); void Ret(IR::Opcode& inst);
    void Push(IR::Opcode& inst);

    void run();
    void resizeReg(int dst);

    void pushFrame(Frame newFrame);
    void popFrame();

    const IR::Opcode& getInstruction(int pc);
    Value& getVariable(int offset);

    void runCmp(IR::Opcode& inst);

    void print();
};

#endif