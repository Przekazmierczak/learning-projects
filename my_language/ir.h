// -----------------------------------------------------------------------------
// IR (Intermediate Representation)
// -----------------------------------------------------------------------------
// Transforms AST into a low-level instruction sequence.
// Handles:
//  - Expressions (arithmetic, logical)
//  - Control flow (if, while, for)
//  - Functions (declaration, calls, returns)
//  - Variable scoping and register allocation
//
// Uses a simple register-based virtual machine model.
// -----------------------------------------------------------------------------

#ifndef IR_H
#define IR_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "parser.h"
#include "stdlib.h"

// -----------------------------------------------------------------------------
// IR Structure
// -----------------------------------------------------------------------------
// Stores instructions, function metadata, variable scopes,
// and register allocation state.
// -----------------------------------------------------------------------------
struct IR {
    // -------------------------------------------------------------------------
    // Opcode Structure
    // -------------------------------------------------------------------------
    // Represents a single instruction in IR.
    // Contains operation type and optional operands.
    // -------------------------------------------------------------------------
    struct Opcode {
        enum Type {
            Int, Bool, String,
            Add, Sub, Mul, Div,
            Neg,
            Assign, Load,
            CmpEq, CmpNEq, CmpGt, CmpLs, CmpGtEq, CmpLsEq,
            Jmp, JmpZ, JmpNZ,
            Call, Ret,
            Push
        };

        // Mandatory operands
        Type operation;

        // Optional operands (depending on opcode)
        int dst;
        int src1;
        int src2;
        int val;
        bool bval;
        std::string name;
    };

    // -------------------------------------------------------------------------
    // Function Metadata
    // -------------------------------------------------------------------------
    // Stores information required to execute a function.
    // -------------------------------------------------------------------------
    struct FunctionMeta {
        int startPC;
        int argsCount;
        int regCount;
        int varCount;

        bool native;

        FunctionMeta() = default; 
        FunctionMeta(int pc, int args, int reg, bool newNative) :
            startPC(pc), argsCount(args), regCount(reg), varCount(0), native(newNative) {};
    };

    // -------------------------------------------------------------------------
    // Context Type
    // -------------------------------------------------------------------------
    // Determines whether IR generation is global or inside a function.
    // -----------------------------------------------------------------------------
    enum class ContextType {
        Default,
        FunDeclaration
    };

    ContextType currContext = ContextType::Default;

    // -------------------------------------------------------------------------
    // Register Allocation
    // -------------------------------------------------------------------------
    int currGlobalReg = 0;
    int currLocalReg = 0;

    // -------------------------------------------------------------------------
    // Variable Scopes
    // -------------------------------------------------------------------------
    std::vector<std::unordered_map<std::string, int>> varMap;
    int nextVarIndex = 0;
    std::vector<std::unordered_map<std::string, int>> functionsVarMap;
    int nextFunctionsVarIndex = 0;

    // -------------------------------------------------------------------------
    // Instruction Storage
    // -------------------------------------------------------------------------
    std::vector<Opcode> instructions;
    std::vector<Opcode> functionsInstructions;

    // -------------------------------------------------------------------------
    // Function Registry
    // -------------------------------------------------------------------------
    std::unordered_map<std::string, int> functionsNameMap;
    std::vector<FunctionMeta> functionsMetaMap;

    // -------------------------------------------------------------------------
    // Register Allocation Helper
    // -------------------------------------------------------------------------
    int getNextIndex() {
        if (currContext == ContextType::Default) {
            return currGlobalReg++;
        } else {
            return currLocalReg++;
        }
    }

    // -------------------------------------------------------------------------
    // Constructor
    // -------------------------------------------------------------------------
    // Initializes IR, loads native functions, and generates instructions.
    // -------------------------------------------------------------------------
    IR(const std::unique_ptr<Parser::NodeAST>& node) {
        functionsNameMap["__main__"] = 0;
        functionsMetaMap.push_back(FunctionMeta(0, 0, 0, false));

        // Load native functions
        for (int i = 0; i < nativeFunctions.size(); i ++) {
            addFunctionMeta(
                nativeFunctions[i].name,
                FunctionMeta(i, nativeFunctions[i].argsCount, 0, true)
            );
        }

        generate(node);

        functionsMetaMap[0].regCount = currGlobalReg;
        functionsMetaMap[0].varCount = nextVarIndex;
    }

    // -------------------------------------------------------------------------
    // Main IR Generation
    // -------------------------------------------------------------------------
    int generate(const std::unique_ptr<Parser::NodeAST>& node);

    // -------------------------------------------------------------------------
    // Literal Instructions
    // -------------------------------------------------------------------------
    int addIntInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addBoolInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addStringInstructions(const std::unique_ptr<Parser::NodeAST>& node);

    // -------------------------------------------------------------------------
    // Expression & Block Handling
    // -------------------------------------------------------------------------
    void addBlockInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addNegInstructions(const std::unique_ptr<Parser::NodeAST>& node);

    // -------------------------------------------------------------------------
    // Variables
    // -------------------------------------------------------------------------
    void addAssignInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addVarInstructions(const std::unique_ptr<Parser::NodeAST>& node);

    // -------------------------------------------------------------------------
    // Control Flow
    // -------------------------------------------------------------------------
    void addIfInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    void addWhileInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    void addForInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addAndOrInstructions(const std::unique_ptr<Parser::NodeAST>& node, bool ifAnd);

    // -------------------------------------------------------------------------
    // Declarations
    // -------------------------------------------------------------------------
    void addDecInstructions(const std::unique_ptr<Parser::NodeAST>& node);

    // -------------------------------------------------------------------------
    // Functions
    // -------------------------------------------------------------------------
    void addFunInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addCallInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addRetInstructions(const std::unique_ptr<Parser::NodeAST>& node);

    // -------------------------------------------------------------------------
    // Helpers
    // -------------------------------------------------------------------------
    int addConst(int dst, int val);
    int addConst(int dst, bool val);

    void pushInstruction(Opcode instruction);
    std::vector<Opcode>& currInstructionArray();

    std::vector<std::unordered_map<std::string, int>>& currVarMap();
    int getNextVarIndex();

    bool findInMap(int index, const std::string& name);
    int findInMaps(const std::string& name);

    void addFunctionMeta(std::string name, FunctionMeta functionMeta);

    // -------------------------------------------------------------------------
    // Debug
    // -------------------------------------------------------------------------
    void print();
};

// -----------------------------------------------------------------------------
// Opcode Printer
// -----------------------------------------------------------------------------
// Prints IR instructions for debugging.
// -----------------------------------------------------------------------------
std::ostream& operator << (std::ostream& cout, IR::Opcode& inst);

#endif