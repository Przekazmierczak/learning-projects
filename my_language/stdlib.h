// -----------------------------------------------------------------------------
// Standard Library (Native Functions)
// -----------------------------------------------------------------------------
// Provides built-in functions available to the VM at runtime.
//
// Defines native functions such as printing, input handling, and type
// conversions. These functions are mapped and callable via the VM.
//
// Each function operates on a vector of Value objects and returns a Value.
// -----------------------------------------------------------------------------

#ifndef STDLIB_H
#define STDLIB_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "helper.h"
#include "value.h"

// -----------------------------------------------------------------------------
// Native Function Type
// -----------------------------------------------------------------------------
// Defines function pointer signature for native functions.
// -----------------------------------------------------------------------------
using Func = Value(*)(const std::vector<Value>&);

// -----------------------------------------------------------------------------
// Native Function Metadata
// -----------------------------------------------------------------------------
// Stores name, argument count, and function pointer.
// -----------------------------------------------------------------------------
struct NativeFunction {
    std::string name; // Function name
    int argsCount; // Number of expected arguments
    Func fun; // Function pointer
};

// -----------------------------------------------------------------------------
// Native Functions Table
// -----------------------------------------------------------------------------
// Stores all available native functions accessible by the VM.
// -----------------------------------------------------------------------------
extern std::vector<NativeFunction> nativeFunctions;

// -----------------------------------------------------------------------------
// Native Function Declarations
// -----------------------------------------------------------------------------
Value print(const std::vector<Value>& vars);
Value printn(const std::vector<Value>& vars);
Value input(const std::vector<Value>& vars);
Value inputInt(const std::vector<Value>& vars);
Value toInt(const std::vector<Value>& vars);
Value toBool(const std::vector<Value>& vars);
Value toString(const std::vector<Value>& vars);

// -----------------------------------------------------------------------------
// Helper Function
// -----------------------------------------------------------------------------
// Converts Value to string representation for printing.
// -----------------------------------------------------------------------------
std::string printHelper(const Value& var);

#endif