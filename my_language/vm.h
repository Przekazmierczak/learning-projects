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

    std::vector<Variable> registers;
    std::vector<std::unordered_map<std::string, int>> maps;

    int pc = 0;

    VM(std::vector<IR::OP> instructions) {
        run(instructions);
    }

    void run(const std::vector<IR::OP>& instructions);
    void resizeReg(int dst);
    bool isInt(Variable var);
    bool isBool(Variable var);
    bool findInMap(int index, const std::string& name);
    int findInMaps(const std::string& name);
    void runCmp(const std::vector<IR::OP>& instructions);
};



#endif