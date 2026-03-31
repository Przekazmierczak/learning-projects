// -----------------------------------------------------------------------------
// Helper Utilities
// -----------------------------------------------------------------------------
// Provides common utility functions for error handling.
//
// Contains overloaded throwError functions to report errors with optional
// line and position information. All functions terminate execution by throwing
// an exception after printing the error message.
// -----------------------------------------------------------------------------

#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <string>
#include <stdexcept>

// -----------------------------------------------------------------------------
// Throw Error
// -----------------------------------------------------------------------------
[[noreturn]] void throwError(std::string errorMsg);
[[noreturn]] void throwError(std::string errorMsg, int line);
[[noreturn]] void throwError(std::string errorMsg, int line, int position);

#endif