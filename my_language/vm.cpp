// -----------------------------------------------------------------------------
// VM Implementation
// -----------------------------------------------------------------------------
// Implements execution logic for the Virtual Machine.
//
// Each function corresponds to a specific IR opcode and performs
// operations on registers, stack frames, and control flow.
//
// The VM uses a dispatch table to efficiently execute instructions.
// -----------------------------------------------------------------------------

#include "vm.h"

// -----------------------------------------------------------------------------
// Main Execution Loop
// -----------------------------------------------------------------------------
// Iterates through instructions and executes them using dispatch table.
// Returns: none
// Throws: Runtime error if execution ends inside a function frame
// -----------------------------------------------------------------------------
void VM::run() {
    while (pc < (*frames.back().instructions).size()) {
        IR::Opcode inst = getInstruction(pc);
        (this->*dispatch[inst.operation])(inst);
    }

    if (frames.size() > 1) {
        throwError("Program termiante from function");
    }
}

// -----------------------------------------------------------------------------
// Dispatch Table Initialization
// -----------------------------------------------------------------------------
VM::Funct VM::dispatch[] = {
    &VM::Int, &VM::Bool, &VM::String,
    &VM::Add, &VM::Sub, &VM::Mul, &VM::Div,
    &VM::Neg,
    &VM::Assign, &VM::Load,
    &VM::CmpEq, &VM::CmpNEq, &VM::CmpGt, &VM::CmpLs, &VM::CmpGtEq, &VM::CmpLsEq,
    &VM::Jmp, &VM::JmpZ, &VM::JmpNZ,
    &VM::Call, &VM::Ret,
    &VM::Push
};

// -----------------------------------------------------------------------------
// Load Integer Constant
// -----------------------------------------------------------------------------
// Parameters:
//   inst - IR instruction containing destination and integer value
// Returns: none
// -----------------------------------------------------------------------------
void VM::Int(IR::Opcode& inst) {
    getVariable(inst.dst).value = inst.val;
    pc++;
}

// -----------------------------------------------------------------------------
// Load Boolean Constant
// -----------------------------------------------------------------------------
// Parameters:
//   inst - IR instruction containing destination and boolean value
// Returns: none
// -----------------------------------------------------------------------------
void VM::Bool(IR::Opcode& inst) {
    getVariable(inst.dst).value = inst.bval;
    pc++;
}

// -----------------------------------------------------------------------------
// Load String Constant
// -----------------------------------------------------------------------------
// Parameters:
//   inst - IR instruction containing destination and string value
// Returns: none
// -----------------------------------------------------------------------------
void VM::String(IR::Opcode& inst) {
    getVariable(inst.dst).value = inst.name;
    pc++;
}

// -----------------------------------------------------------------------------
// Addition Operation
// -----------------------------------------------------------------------------
// Supports int + int and string + string
// Parameters:
//   inst - IR instruction containing:
//          dst  -> destination register index
//          src1 -> first operand register index
//          src2 -> second operand register index
// Throws: Error on incompatible types
// -----------------------------------------------------------------------------
void VM::Add(IR::Opcode& inst) {
    auto& src1 = getVariable(inst.src1);
    auto& src2 = getVariable(inst.src2);

    if (src1.isInt() && src2.isInt()) {
        getVariable(inst.dst).value = src1.getInt() + src2.getInt();
    } else if (src1.isString() && src2.isString()) {
        getVariable(inst.dst).value = src1.getString() + src2.getString();
    } else {
        throwError("Incorrect types for Add");
    }
    pc++;
}

// -----------------------------------------------------------------------------
// Subtraction Operation
// -----------------------------------------------------------------------------
// Supports int - int
// Parameters:
//   inst - IR instruction containing:
//          dst  -> destination register index
//          src1 -> first operand register index
//          src2 -> second operand register index
// Throws: Error on incompatible types
// -----------------------------------------------------------------------------
void VM::Sub(IR::Opcode& inst) {
    auto& src1 = getVariable(inst.src1);
    auto& src2 = getVariable(inst.src2);

    if (src1.isInt() && src2.isInt()) {
        getVariable(inst.dst).value = src1.getInt() - src2.getInt();
    } else {
        throwError("Incorrect types for Sub");
    }
    pc++;
}

// -----------------------------------------------------------------------------
// Multiplication Operation
// -----------------------------------------------------------------------------
// Supports int * int
// Parameters:
//   inst - IR instruction containing:
//          dst  -> destination register index
//          src1 -> first operand register index
//          src2 -> second operand register index
// Throws: Error on incompatible types
// -----------------------------------------------------------------------------
void VM::Mul(IR::Opcode& inst) {
    auto& src1 = getVariable(inst.src1);
    auto& src2 = getVariable(inst.src2);

    if (src1.isInt() && src2.isInt()) {
        getVariable(inst.dst).value = src1.getInt() * src2.getInt();
    } else {
        throwError("Incorrect types for Mul");
    }
    pc++;
}

// -----------------------------------------------------------------------------
// Division Operation
// -----------------------------------------------------------------------------
// Supports int / int
// Parameters:
//   inst - IR instruction containing:
//          dst  -> destination register index
//          src1 -> first operand register index
//          src2 -> second operand register index
// Throws:
//   Error on division by zero or invalid types
//   Error on incompatible types
// -----------------------------------------------------------------------------
void VM::Div(IR::Opcode& inst) {
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

// -----------------------------------------------------------------------------
// Negation Operation
// -----------------------------------------------------------------------------
// Supports -int
// Parameters:
//   inst - IR instruction containing:
//          dst  -> destination register index
//          src1 -> operand register index
// Throws: Error on incompatible types
// -----------------------------------------------------------------------------
void VM::Neg(IR::Opcode& inst) {
    auto& src1 = getVariable(inst.src1);

    if (src1.isInt()) {
        getVariable(inst.dst).value = -src1.getInt();
    } else {
        throwError("Incorrect types for Neg");
    }
    pc++;
}

// -----------------------------------------------------------------------------
// Assign Variable
// -----------------------------------------------------------------------------
// Copies register value into variable map
// Parameters:
//   inst - IR instruction containing:
//          dst  -> source register index (value to copy)
//          src1 -> variable index in current frame
// -----------------------------------------------------------------------------
void VM::Assign(IR::Opcode& inst) {
    frames.back().varMap[inst.src1] = getVariable(inst.dst);
    pc++;
}

// -----------------------------------------------------------------------------
// Load Variable
// -----------------------------------------------------------------------------
// Loads variable from frame into register
// Parameters:
//   inst - IR instruction containing:
//          dst  -> destination register index
//          src1 -> variable index in current frame
// -----------------------------------------------------------------------------
void VM::Load(IR::Opcode& inst) {
    getVariable(inst.dst) = frames.back().varMap[inst.src1];
    pc++;
}

// -----------------------------------------------------------------------------
// Function Call
// -----------------------------------------------------------------------------
// Handles both native and user-defined functions
// Parameters:
//   inst - IR instruction containing:
//          dst  -> register to store return value
//          src1 -> function index in functionsMap
// Throws: Error if function metadata is invalid
// -----------------------------------------------------------------------------
void VM::Call(IR::Opcode& inst) {
    IR::FunctionMeta funMeta = functionsMap[inst.src1];
    
    if (funMeta.native) {
        std::vector<Value> args;
        for (int i = 0; i < funMeta.argsCount; i++) {
            args.push_back(registers[frames.back().topStack + i]);
        }
        getVariable(inst.dst) = nativeFunctions[funMeta.startPC].fun(args);

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

// -----------------------------------------------------------------------------
// Return from Function
// -----------------------------------------------------------------------------
// Restores previous frame and passes return value
// Parameters:
//   inst - IR instruction containing:
//          dst  -> register holding return value
// -----------------------------------------------------------------------------
void VM::Ret(IR::Opcode& inst) {
    Value result = getVariable(inst.dst);
    int returnReg = frames.back().returnReg;
    int returnPc = frames.back().returnPC;

    frames.pop_back();
    getVariable(returnReg) = result;

    pc = returnPc;
    registersEnd = frames.back().topStack;
}

// -----------------------------------------------------------------------------
// Push Argument onto Stack
// -----------------------------------------------------------------------------
// Parameters:
//   inst - IR instruction containing:
//          src1 -> register index of value to push onto stack
// -----------------------------------------------------------------------------
void VM::Push(IR::Opcode& inst) {
    resizeReg(registersEnd);
    registers[registersEnd] = getVariable(inst.src1);
    registersEnd++;
    pc++;
}

// -----------------------------------------------------------------------------
// Comparison Operations
// -----------------------------------------------------------------------------
// Parameters:
//   inst - IR instruction containing:
//          dst  -> destination register for boolean result
//          src1 -> first operand register index
//          src2 -> second operand register index
// -----------------------------------------------------------------------------
void VM::CmpEq(IR::Opcode& inst) {
    runCmp(inst);
    pc++;
}

void VM::CmpNEq(IR::Opcode& inst) {
    runCmp(inst);
    pc++;
}

void VM::CmpGt(IR::Opcode& inst) {
    runCmp(inst);
    pc++;
}

void VM::CmpLs(IR::Opcode& inst) {
    runCmp(inst);
    pc++;
}

void VM::CmpGtEq(IR::Opcode& inst) {
    runCmp(inst);
    pc++;
}

void VM::CmpLsEq(IR::Opcode& inst) {
    runCmp(inst);
    pc++;
}

// -----------------------------------------------------------------------------
// Unconditional Jump
// -----------------------------------------------------------------------------
// Parameters:
//   inst - IR instruction containing:
//          dst -> target instruction index (program counter)
// -----------------------------------------------------------------------------
void VM::Jmp(IR::Opcode& inst) {
    pc = inst.dst;
}

// -----------------------------------------------------------------------------
// Jump If Zero / False
// -----------------------------------------------------------------------------
// Parameters:
//   inst - IR instruction containing:
//          src1 -> condition register index
//          dst  -> target instruction index (program counter)
// -----------------------------------------------------------------------------
void VM::JmpZ(IR::Opcode& inst) {
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

// -----------------------------------------------------------------------------
// Jump If Not Zero / True
// -----------------------------------------------------------------------------
// Parameters:
//   inst - IR instruction containing:
//          src1 -> condition register index
//          dst  -> target instruction index (program counter)
// -----------------------------------------------------------------------------
void VM::JmpNZ(IR::Opcode& inst) {
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

// -----------------------------------------------------------------------------
// Resize Register Storage
// -----------------------------------------------------------------------------
// Parameters:
//   dst - required register index
// -----------------------------------------------------------------------------
void VM::resizeReg(int dst) {
    if (dst >= registers.size()) {
        registers.resize(dst + 1);
    }
}

// -----------------------------------------------------------------------------
// Fetch Instruction
// -----------------------------------------------------------------------------
// Parameters:
//   pc - program counter index of instruction to fetch
// Returns:
//   Reference to the IR instruction at given pc
// -----------------------------------------------------------------------------
const IR::Opcode& VM::getInstruction(int pc) {
    return (*frames.back().instructions)[pc];
}


// -----------------------------------------------------------------------------
// Access Register Variable
// -----------------------------------------------------------------------------
// Throws:
//   Error if index is out of bounds
// -----------------------------------------------------------------------------
Value& VM::getVariable(int offset) {
    int index = frames.back().bottomStack + offset;
    if (index >= registers.size()) {
        throwError("Index out of registers size in getVariable");
    }
    return registers[index];
}

// -----------------------------------------------------------------------------
// Comparison Logic
// -----------------------------------------------------------------------------
// Handles all comparison operations based on opcode type
//   inst - IR instruction containing:
//          dst  -> destination register for result
//          src1 -> first operand register index
//          src2 -> second operand register index
//          operation -> comparison type (e.g., Eq, Gt, etc.)
// Throws: Error on incompatible types
// -----------------------------------------------------------------------------
void VM::runCmp(IR::Opcode& inst) {
    auto& src1 = getVariable(inst.src1);
    auto& src2 = getVariable(inst.src2);

    if (src1.isInt() && getVariable(inst.src2).isInt()) {
        switch (inst.operation) {
            
            case IR::Opcode::Type::CmpEq:
                getVariable(inst.dst).value = src1.getInt() == src2.getInt();
                break;

            case IR::Opcode::Type::CmpNEq:
                getVariable(inst.dst).value = src1.getInt() != src2.getInt();
                break;

            case IR::Opcode::Type::CmpGt:
                getVariable(inst.dst).value = src1.getInt() > src2.getInt();
                break;

            case IR::Opcode::Type::CmpLs:
                getVariable(inst.dst).value = src1.getInt() < src2.getInt();
                break;
            
            case IR::Opcode::Type::CmpGtEq:
                getVariable(inst.dst).value = src1.getInt() >= src2.getInt();
                break;
            
            case IR::Opcode::Type::CmpLsEq:
                getVariable(inst.dst).value = src1.getInt() <= src2.getInt();
                break;

            default:
                throwError("Incorrect types for Cmp");
        }
    } else if (src1.isBool() && src2.isBool()) {
        switch (inst.operation) {
            
            case IR::Opcode::Type::CmpEq:
                getVariable(inst.dst).value = src1.getBool() == src2.getBool();
                break;

            case IR::Opcode::Type::CmpNEq:
                getVariable(inst.dst).value = src1.getBool() != src2.getBool();
                break;

            default:
                throwError("Incorrect types for Cmp");
        }
    } else {
        throwError("Incorrect types for Cmp");
    }
}

// -----------------------------------------------------------------------------
// Debug Print Registers
// -----------------------------------------------------------------------------
void VM::print() {
    for (int i = 0; i < registers.size(); i++) {
        if (registers[i].isNaN()) {
            std::cout << "r" << i << "=[Nan],";
        }
        if (registers[i].isInt()) {
            std::cout << "r" << i << "=[int, " << std::get<int>(registers[i].value) << "],";
        }
        if (registers[i].isBool()) {
            std::cout << "r" << i << "=[bool, " << std::get<bool>(registers[i].value) << "],";
        }
        if (registers[i].isString()) {
            std::cout << "r" << i << "=[string, \"" << std::get<std::string>(registers[i].value) << "\"],";
        }
    }
    std::cout << std::endl;
}