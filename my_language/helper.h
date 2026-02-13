#ifndef HELPER_H
#define HELPER_H

#include <iostream>
#include <string>
#include <stdexcept>

[[noreturn]] void throwError(std::string errorMsg);
[[noreturn]] void throwError(std::string errorMsg, int line);
[[noreturn]] void throwError(std::string errorMsg, int line, int position);

#endif