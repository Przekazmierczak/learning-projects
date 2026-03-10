#ifndef STDLIB_H
#define STDLIB_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "helper.h"
#include "variable.h"

using NativeFun = Variable(*)(std::vector<Variable>);

extern std::unordered_map<std::string, int> nativeFunctionsNameMap;
extern NativeFun nativeFunctions[];
extern std::vector<FunctionMeta> nativeFunctionsMetaMap;

Variable print(std::vector<Variable> vars);

#endif