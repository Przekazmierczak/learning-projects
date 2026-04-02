// -----------------------------------------------------------------------------
// Standard Library Implementation
// -----------------------------------------------------------------------------
// Implements native functions used by the VM.
//
// Includes I/O operations and type conversions.
// -----------------------------------------------------------------------------

#include "stdlib.h"

// -----------------------------------------------------------------------------
// Native Functions Table Initialization
// -----------------------------------------------------------------------------
std::vector<NativeFunction> nativeFunctions = {
    {"print", 1, print},
    {"printn", 1, printn},
    {"input", 0, input},
    {"inputInt", 0, inputInt},
    {"toInt", 1, toInt},
    {"toBool", 1, toBool},
    {"toString", 1, toString}
};

// -----------------------------------------------------------------------------
// Print (No Newline)
// -----------------------------------------------------------------------------
// Prints value without newline.
// Parameters:
//   vars - vector containing one Value to print
// Returns: Empty Value (NaN)
// -----------------------------------------------------------------------------
Value print(const std::vector<Value>& vars) {
    std::cout << printHelper(vars[0]);
    return Value();
}

// -----------------------------------------------------------------------------
// Print (With Newline)
// -----------------------------------------------------------------------------
// Prints value followed by newline.
// Parameters:
//   vars - vector containing one Value to print
// Returns: Empty Value (NaN)
// -----------------------------------------------------------------------------
Value printn(const std::vector<Value>& vars) {
    std::cout << printHelper(vars[0]) << std::endl;
    return Value();
}

// -----------------------------------------------------------------------------
// Input (String)
// -----------------------------------------------------------------------------
// Reads a full line from standard input.
// Parameters:
//   vars - unused
// Returns: Value containing input string
// -----------------------------------------------------------------------------
Value input(const std::vector<Value>& vars) {
    std::string input;
    std::getline(std::cin, input);
    return Value(input);
}

// -----------------------------------------------------------------------------
// Input (Integer)
// -----------------------------------------------------------------------------
// Reads integer from standard input.
// Parameters:
//   vars - unused
// Returns: Value containing integer
// -----------------------------------------------------------------------------
Value inputInt(const std::vector<Value>& vars) {
    int input;
    std::cin >> input;
    return Value(input);
}

// -----------------------------------------------------------------------------
// Convert to Integer
// -----------------------------------------------------------------------------
// Converts Value to integer if possible.
// Parameters:
//   vars - vector containing one Value
// Returns: Value containing integer
// Throws: Error if conversion fails or type is invalid
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Convert to Boolean
// -----------------------------------------------------------------------------
// Converts Value to boolean if possible.
// Parameters:
//   vars - vector containing one Value
// Returns: Value containing boolean
// Throws: Error if conversion fails or type is invalid
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Convert to String
// -----------------------------------------------------------------------------
// Converts Value to string representation.
// Parameters:
//   vars - vector containing one Value
// Returns: Value containing string
// Throws: Error if type is invalid
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Print Helper
// -----------------------------------------------------------------------------
// Converts Value to printable string representation.
// Parameters:
//   var - Value to convert
// Returns: String representation of Value
// Throws: Error if type is unknown
// -----------------------------------------------------------------------------
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