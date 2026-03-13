#include "stdlib.h"

std::vector<NativeFunction> nativeFunctions = {
    {"print", 1, print},
    {"printn", 1, printn},
    {"input", 0, input},
    {"inputInt", 0, inputInt},
    {"toInt", 1, toInt},
    {"toBool", 1, toBool},
    {"toString", 1, toString}
};

Value print(const std::vector<Value>& vars) {
    std::cout << printHelper(vars[0]);
    return Value();
}

Value printn(const std::vector<Value>& vars) {
    std::cout << printHelper(vars[0]) << std::endl;
    return Value();
}

Value input(const std::vector<Value>& vars) {
    std::string input;
    std::getline(std::cin, input);
    return Value(input);
}

Value inputInt(const std::vector<Value>& vars) {
    int input;
    std::cin >> input;
    return Value(input);
}

Value toInt(const std::vector<Value>& vars) {
    const Value& var = vars[0];

    if (var.isInt()) {
        return var;
    }
    
    if (var.isBool()) {
        int number = var.getBool();
        return Value(number);
    }
    
    if (var.isString()) {
        try {
            return Value(std::stoi(var.getString()));
        } catch (const std::invalid_argument& e) {
            throwError("String cannot convert to int");
        } catch (const std::out_of_range& e) {
            throwError("Number out of range");
        }
    }

    if (var.isNaN()) {
        throwError("NaN cannot be converted to int");
    }

    throwError("Invalid type in toInt()");
}

Value toBool(const std::vector<Value>& vars) {
    const Value& var = vars[0];

    if (var.isInt()) {
        return Value(var.getInt() != 0);
    }
    
    if (var.isBool()) {
        return var;
    }
    
    if (var.isString()) {
        bool res;
        if (var.getString() == "true") {
            res = true;
        } else if (var.getString() == "false") {
            res = false;
        } else {
            throwError("String cannot convert to bool");
        }
        return Value(res);
    }

    if (var.isNaN()) {
        throwError("NaN cannot convert to bool");
    }

    throwError("Invalid type in toBool()");
}

Value toString(const std::vector<Value>& vars) {
    const Value& var = vars[0];

    if (var.isInt()) {
        std::string str = std::to_string(var.getInt());
        return Value(str);
    }
    
    if (var.isBool()) {
        std::string str = var.getBool() ? "true" : "false";
        return Value(str);
    }
    
    if (var.isString()) {
        return var;
    }

    if (var.isNaN()) {
        return Value("NaN");
    }

    throwError("Invalid type in toString()");
}

std::string printHelper(const Value& var) {
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