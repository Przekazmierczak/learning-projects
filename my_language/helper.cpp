// -----------------------------------------------------------------------------
// Helper Utilities Implementation
// -----------------------------------------------------------------------------
// Implements error handling functions used across the system.
// Each function prints an error message and throws an exception.
// -----------------------------------------------------------------------------

#include "helper.h"

// -----------------------------------------------------------------------------
// Throw Error (Message Only)
// -----------------------------------------------------------------------------
// Parameters:
//   errorMsg - description of the error
// Returns: none
// Throws: std::invalid_argument with provided message
// -----------------------------------------------------------------------------
[[noreturn]] void throwError(std::string errorMsg) {
    std::cerr << errorMsg << std::endl;
    throw std::invalid_argument(errorMsg);
}

// -----------------------------------------------------------------------------
// Throw Error (With Line)
// -----------------------------------------------------------------------------
// Parameters:
//   errorMsg - description of the error
//   line     - line number where error occurred
// Returns: none
// Throws: std::invalid_argument with formatted message
// -----------------------------------------------------------------------------
[[noreturn]] void throwError(std::string errorMsg, int line) {
    std::string newErrorMsg = errorMsg + "; Line: " + std::to_string(line);
    std::cerr << newErrorMsg << std::endl;
    throw std::invalid_argument(newErrorMsg);
}


// -----------------------------------------------------------------------------
// Throw Error (With Line and Position)
// -----------------------------------------------------------------------------
// Parameters:
//   errorMsg - description of the error
//   line     - line number where error occurred
//   position - character position in line
// Returns: none
// Throws: std::invalid_argument with formatted message
// -----------------------------------------------------------------------------
[[noreturn]] void throwError(std::string errorMsg, int line, int position) {
    std::string newErrorMsg = errorMsg + "; Line: " + std::to_string(line) + ", Position: " + std::to_string(position);
    std::cerr << newErrorMsg << std::endl;
    throw std::invalid_argument(newErrorMsg);
}