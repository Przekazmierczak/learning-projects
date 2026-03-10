#ifndef VARIABLE_H
#define VARIABLE_H

#include <variant>

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

#endif