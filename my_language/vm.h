#ifndef VM_H
#define VM_H

#include <iostream>
#include <vector>
#include <unordered_map>

#include "ir.h"
#include "helper.h"

struct VM {
    enum class Type {
        NaN,
        Int,
        Bool
    };

    struct Variable {
        Type type = Type::NaN;

        union {
            int i;
            bool b;
        };

        Variable() {};
        Variable(int val) : type(Type::Int), i(val) {};
        Variable(bool val) : type(Type::Bool), b(val) {};

        bool operator==(const Variable& other) const {
            if (type == other.type) {
                switch (type) {
                    
                    case Type::Int:
                        return i == other.i;

                    case Type::Bool:
                        return b == other.b;

                    case Type::NaN:
                        return true;
                }
            }
            return false;
        }
    };

    struct Frame {
        std::vector<IR::OP>* instructions;
        int returnPC;
        int returnReg;
        int bottomStack;
        int topStack;
        std::vector<std::unordered_map<std::string, Variable>> maps;
    };

    std::vector<IR::OP>& mainInstructions;
    std::vector<IR::OP>& functionsInstructions;
    std::unordered_map<std::string, IR::FunctionMeta>& functionsMap;

    std::vector<Variable> registers;
    std::vector<Frame> frames;

    int pc = 0;
    int currBottomStack;
    int currTopStack;
    int registersEnd;

    VM(
        std::vector<IR::OP>& newMainInstructions,
        std::vector<IR::OP>& newFunctionsInstructions,
        std::unordered_map<std::string, IR::FunctionMeta>& newFunctionsMap
    ) : 
        mainInstructions(newMainInstructions),
        functionsInstructions(newFunctionsInstructions),
        functionsMap(newFunctionsMap)
    {

        Frame mainFrame;
        mainFrame.instructions = &mainInstructions;
        mainFrame.returnPC = -1;
        mainFrame.bottomStack = 0;
        mainFrame.topStack = functionsMap["__main__"].argsCount + functionsMap["__main__"].regCount;

        frames.push_back(mainFrame);
        registersEnd = mainFrame.topStack;

        run();
    }

    void run();
    void resizeReg(int dst);

    void pushFrame(Frame newFrame);
    void popFrame();

    const IR::OP& getInstruction(int pc);
    Variable& getVariable(int offset);

    bool isInt(Variable var);
    bool isBool(Variable var);

    bool findInMap(int index, const std::string& name);
    int findInMaps(const std::string& name);

    void runCmp();
};



#endif