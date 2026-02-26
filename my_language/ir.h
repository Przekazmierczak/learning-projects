#ifndef IR_H
#define IR_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "parser.h"

struct IR {
    struct OP {
        enum class Type {
            Dec,
            Int, Bool,
            Add, Sub, Mul, Div,
            Neg,
            Assign, Load,
            CmpEq, CmpNEq, CmpGt, CmpLs, CmpGtEq, CmpLsEq,
            Jmp, JmpZ, JmpNZ,
            Block, Deblock,
            And, Or,
            Call, Ret,
            Mov, Push
        };

        // mandatory
        Type operation;

        // optional
        int dst;
        int src1;
        int src2;
        int val;
        bool bval;
        std::string name;
    };

    enum class ContextType {
        Default,
        FunDeclaration
    };

    struct FunctionMeta {
        int startPC;
        int argsCount;
        int regCount;

        FunctionMeta() = default; 
        FunctionMeta(int pc, int args, int reg) : startPC(pc), argsCount(args), regCount(reg) {};
    };

    ContextType currContext = ContextType::Default;

    int currGlobalReg = 0;
    int currLocalReg = 0;

    std::vector<OP> instructions;
    std::vector<OP> functionsInstructions;

    std::unordered_map<std::string, FunctionMeta> functionsMap;

    int getNextIndex() {
        if (currContext == ContextType::Default) {
            return currGlobalReg++;
        } else {
            return currLocalReg++;
        }
    }

    IR(const std::unique_ptr<Parser::NodeAST>& node) {
        generate(node);
        functionsMap["__main__"] = FunctionMeta(0, 0, currGlobalReg);
    }

    int generate(const std::unique_ptr<Parser::NodeAST>& node);

    void addIfInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    void addWhileInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    void addForInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addAndOrInstructions(const std::unique_ptr<Parser::NodeAST>& node, bool ifAnd);

    void addFunInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addCallInstructions(const std::unique_ptr<Parser::NodeAST>& node);

    int addConst(int dst, int val);
    int addConst(int dst, bool val);
    
    void pushBlock();
    void popBlock();

    void pushInstruction(OP instruction);
    std::vector<OP>& currInstructionArray();

    void addFunctionMeta(std::string name, FunctionMeta functionMeta);
    void print();
};

std::ostream& operator << (std::ostream& cout, IR::OP& inst);

#endif