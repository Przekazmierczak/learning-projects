#include "stdlib.h"

std::vector<NativeFunction> nativeFunctions = {
    {"print", 1, print},
    {"printn", 1, printn}
};

Variable print(const std::vector<Variable>& vars) {
    std::cout << printHelper(vars[0]);
    return Variable();
}

Variable printn(const std::vector<Variable>& vars) {
    std::cout << printHelper(vars[0]) << std::endl;
    return Variable();
}

std::string printHelper(const Variable& var) {
    if (var.isInt()) {
        return std::to_string(var.getInt());
    } else if(var.isBool()) {
        return (var.getBool() ? "true" : "false");
    } else if (var.isNaN()) {
        return "NaN";
    }
    throwError("Unknown type in print");
}