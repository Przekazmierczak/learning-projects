#ifndef VM_H
#define VM_H

#include <iostream>
#include <vector>
#include <unordered_map>

#include "ir.h"
#include "helper.h"

struct VM {
    std::vector<int> registers;
    std::unordered_map<std::string, int> map;

    int pc = 0;

    VM(std::vector<IR::OP> instructions) {
        run(instructions);
    }

    void run(const std::vector<IR::OP>& instructions);
    void resizeReg(int dst);
};

#endif