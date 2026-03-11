#ifndef IR_H
#define IR_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "parser.h"
#include "stdlib.h"

struct IR {
    struct OP {
        enum Type {
            Int, Bool,
            Add, Sub, Mul, Div,
            Neg,
            Assign, Load,
            CmpEq, CmpNEq, CmpGt, CmpLs, CmpGtEq, CmpLsEq,
            Jmp, JmpZ, JmpNZ,
            Call, Ret,
            Push
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

    enum class ContextType {
        Default,
        FunDeclaration
    };

    ContextType currContext = ContextType::Default;

    int currGlobalReg = 0;
    int currLocalReg = 0;

    std::vector<std::unordered_map<std::string, int>> varMap;
    int nextVarIndex = 0;
    std::vector<std::unordered_map<std::string, int>> functionsVarMap;
    int nextFunctionsVarIndex = 0;

    std::vector<OP> instructions;
    std::vector<OP> functionsInstructions;

    std::unordered_map<std::string, int> functionsNameMap;
    std::vector<FunctionMeta> functionsMetaMap;

    int getNextIndex() {
        if (currContext == ContextType::Default) {
            return currGlobalReg++;
        } else {
            return currLocalReg++;
        }
    }

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

    int generate(const std::unique_ptr<Parser::NodeAST>& node);

    int addIntInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addBoolInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    void addBlockInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addNegInstructions(const std::unique_ptr<Parser::NodeAST>& node);

    void addAssignInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addVarInstructions(const std::unique_ptr<Parser::NodeAST>& node);

    void addIfInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    void addWhileInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    void addForInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addAndOrInstructions(const std::unique_ptr<Parser::NodeAST>& node, bool ifAnd);

    void addDecInstructions(const std::unique_ptr<Parser::NodeAST>& node);

    void addFunInstructions(const std::unique_ptr<Parser::NodeAST>& node);
    int addCallInstructions(const std::unique_ptr<Parser::NodeAST>& node);

    int addRetInstructions(const std::unique_ptr<Parser::NodeAST>& node);

    int addConst(int dst, int val);
    int addConst(int dst, bool val);

    void pushInstruction(OP instruction);
    std::vector<OP>& currInstructionArray();

    std::vector<std::unordered_map<std::string, int>>& currVarMap();
    int getNextVarIndex();

    bool findInMap(int index, const std::string& name);
    int findInMaps(const std::string& name);

    void addFunctionMeta(std::string name, FunctionMeta functionMeta);
    void print();
};

std::ostream& operator << (std::ostream& cout, IR::OP& inst);

#endif