// -----------------------------------------------------------------------------
// Virtual Machine (VM)
// -----------------------------------------------------------------------------
// Executes intermediate representation (IR) instructions produced by the compiler.
//
// Implements a stack-based execution model with support for:
// - Arithmetic and logical operations
// - Control flow (jumps, conditionals)
// - Function calls (including native functions)
// - Variable storage via registers and stack frames
//
// Uses a dispatch table to efficiently map opcodes to handler functions.
// Maintains execution state via program counter (pc), register space,
// and a stack of frames for function calls.
// -----------------------------------------------------------------------------

#ifndef VM_H
#define VM_H

#include <iostream>
#include <vector>
#include <unordered_map>

#include "ir.h"
#include "helper.h"
#include "value.h"

// -----------------------------------------------------------------------------
// VM Structure
// -----------------------------------------------------------------------------
// Stores instruction sets, registers, and execution frames.
// Responsible for running IR instructions step-by-step.
// -----------------------------------------------------------------------------
struct VM {
    struct Frame {
        std::vector<IR::Opcode>* instructions; // Pointer to instruction set
        int returnPC; // Program counter to return to after function ends
        int returnReg; // Register to store return value
        int bottomStack; // Start index in register stack
        int topStack; // End index in register stack

        std::vector<Value> varMap; // Local variables
    };

    // -------------------------------------------------------------------------
    // Core VM State
    // -------------------------------------------------------------------------
    std::vector<IR::Opcode>& mainInstructions; // Main program instructions
    std::vector<IR::Opcode>& functionsInstructions; // Functions instruction pool
    std::vector<IR::FunctionMeta>& functionsMap; // Function metadata

    std::vector<Value> registers;  // Register storage
    std::vector<Frame> frames; // Call stack

    int pc = 0; // Program counter
    int currBottomStack; // Current frame bottom
    int currTopStack; // Current frame top
    int registersEnd; // Next free register index
    
    // -------------------------------------------------------------------------
    // Dispatch Table
    // -------------------------------------------------------------------------
    // Maps opcode types to corresponding handler functions.
    // -------------------------------------------------------------------------
    using Funct = void(VM::*)(IR::Opcode& instruction);
    static Funct dispatch[];

    // -------------------------------------------------------------------------
    // Constructor
    // -------------------------------------------------------------------------
    // Initializes the VM with instruction sets and starts execution.
    // -------------------------------------------------------------------------
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

    // -------------------------------------------------------------------------
    // Instruction Handlers
    // -------------------------------------------------------------------------
    void Int(IR::Opcode& inst); void Bool(IR::Opcode& inst); void String(IR::Opcode& inst);
    void Add(IR::Opcode& inst); void Sub(IR::Opcode& inst); void Mul(IR::Opcode& inst); void Div(IR::Opcode& inst);
    void Neg(IR::Opcode& inst);
    void Assign(IR::Opcode& inst); void Load(IR::Opcode& inst);
    void CmpEq(IR::Opcode& inst); void CmpNEq(IR::Opcode& inst); void CmpGt(IR::Opcode& inst);
    void CmpLs(IR::Opcode& inst); void CmpGtEq(IR::Opcode& inst); void CmpLsEq(IR::Opcode& inst);
    void Jmp(IR::Opcode& inst); void JmpZ(IR::Opcode& inst); void JmpNZ(IR::Opcode& inst);
    void Call(IR::Opcode& inst); void Ret(IR::Opcode& inst);
    void Push(IR::Opcode& inst);

    // -------------------------------------------------------------------------
    // Execution Control
    // -------------------------------------------------------------------------
    void run(); // Main execution loop
    void resizeReg(int dst); // Ensure register capacity

    // -------------------------------------------------------------------------
    // Frame Management
    // -------------------------------------------------------------------------
    void pushFrame(Frame newFrame);
    void popFrame();

    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------
    const IR::Opcode& getInstruction(int pc); // Fetch instruction
    Value& getVariable(int offset); // Access register-relative variable

    void runCmp(IR::Opcode& inst); // Shared comparison logic

    // -------------------------------------------------------------------------
    // Debugging
    // -------------------------------------------------------------------------
    void print();
};

#endif