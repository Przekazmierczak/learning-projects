#ifndef STDLIB_H
#define STDLIB_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "helper.h"
#include "variable.h"

using Func = Variable(*)(const std::vector<Variable>&);

struct NativeFunction {
    std::string name;
    int argsCount;
    Func fun;
};

extern std::vector<NativeFunction> nativeFunctions;

Variable print(const std::vector<Variable>& vars);
Variable printn(const std::vector<Variable>& vars);
Variable input(const std::vector<Variable>& vars);
Variable inputInt(const std::vector<Variable>& vars);
Variable toInt(const std::vector<Variable>& vars);
Variable toBool(const std::vector<Variable>& vars);
Variable toString(const std::vector<Variable>& vars);

std::string printHelper(const Variable& var);

#endif