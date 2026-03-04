#ifndef VM_H
#define VM_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <variant>

#include "ir.h"
#include "helper.h"

struct VM {
    // enum class Type {
    //     NaN,
    //     Int,
    //     Bool
    // };

    struct Variable {
        std::variant<std::monostate, int, bool> value;

        Variable() = default;
        Variable(int valInt) : value(valInt) {};
        Variable(bool valBool) : value(valBool) {};

        bool operator==(const Variable& other) const {
            return value == other.value;
        }

        bool isInt() const {
            return std::holds_alternative<int>(value);
        }

        bool isBool() const {
            return std::holds_alternative<bool>(value);
        }

        bool isNaN() const {
            return std::holds_alternative<std::monostate>(value);
        }

        int getInt() const {
            return std::get<int>(value);
        }

        bool getBool() const {
            return std::get<bool>(value);
        }
    };

    struct Frame {
        std::vector<IR::OP>* instructions;
        int returnPC;
        int returnReg;
        int bottomStack;
        int topStack;

        //std::vector<std::unordered_map<std::string, Variable>> maps;
        std::vector<Variable> varMap;
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
        mainFrame.varMap.resize(functionsMap["__main__"].varCount);

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

    // bool findInMap(int index, const std::string& name);
    // int findInMaps(const std::string& name);

    void runCmp();
};



#endif