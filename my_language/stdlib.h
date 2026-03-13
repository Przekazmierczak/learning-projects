#ifndef STDLIB_H
#define STDLIB_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "helper.h"
#include "value.h"

using Func = Value(*)(const std::vector<Value>&);

struct NativeFunction {
    std::string name;
    int argsCount;
    Func fun;
};

extern std::vector<NativeFunction> nativeFunctions;

Value print(const std::vector<Value>& vars);
Value printn(const std::vector<Value>& vars);
Value input(const std::vector<Value>& vars);
Value inputInt(const std::vector<Value>& vars);
Value toInt(const std::vector<Value>& vars);
Value toBool(const std::vector<Value>& vars);
Value toString(const std::vector<Value>& vars);

std::string printHelper(const Value& var);

#endif