#include "helper.h"

[[noreturn]] void throwError(std::string errorMsg) {
    std::cerr << errorMsg << std::endl;
    throw std::invalid_argument(errorMsg);
}

[[noreturn]] void throwError(std::string errorMsg, int line) {
    std::string newErrorMsg = errorMsg + "; Line: " + std::to_string(line);
    std::cerr << newErrorMsg << std::endl;
    throw std::invalid_argument(newErrorMsg);
}

[[noreturn]] void throwError(std::string errorMsg, int line, int position) {
    std::string newErrorMsg = errorMsg + "; Line: " + std::to_string(line) + ", Position: " + std::to_string(position);
    std::cerr << newErrorMsg << std::endl;
    throw std::invalid_argument(newErrorMsg);
}