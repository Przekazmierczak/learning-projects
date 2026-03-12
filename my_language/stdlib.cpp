#include "stdlib.h"

std::vector<NativeFunction> nativeFunctions = {
    {"print", 1, print},
    {"printn", 1, printn},
    {"input", 0, input},
    {"intput", 0, intput},
    {"toint", 1, toint}
};

Variable print(const std::vector<Variable>& vars) {
    std::cout << printHelper(vars[0]);
    return Variable();
}

Variable printn(const std::vector<Variable>& vars) {
    std::cout << printHelper(vars[0]) << std::endl;
    return Variable();
}

Variable input(const std::vector<Variable>& vars) {
    std::string input;
    std::getline(std::cin, input);
    return Variable(input);
}

Variable intput(const std::vector<Variable>& vars) {
    int input;
    std::cin >> input;
    return Variable(input);
}

Variable toint(const std::vector<Variable>& vars) {
    Variable var = vars[0];

    if (var.isInt()) {
        return var;
    }
    
    if (var.isBool()) {
        int number = var.getBool();
        return Variable(number);
    }
    
    if (var.isString()) {
        try {
            return Variable(std::stoi(var.getString()));
        } catch (const std::invalid_argument& e) {
            throwError("Variable (string) can not be converted to int - no digits");
        } catch (const std::out_of_range& e) {
            throwError("Variable (string) can not be converted to int - too big number");
        }
    }

    if (var.isNaN()) {
        throwError("NaN can not be converted to int");
    }

    throwError("Incorrect Variable type in toint()");
}

std::string printHelper(const Variable& var) {
    if (var.isInt()) {
        return std::to_string(var.getInt());
    } else if(var.isBool()) {
        return (var.getBool() ? "true" : "false");
    } else if (var.isNaN()) {
        return "NaN";
    } else if (var.isString()) {
        return var.getString();
    }
    throwError("Unknown type in print");
}