#include "stdlib.h"
#include "vm.h"

std::unordered_map<std::string, int> nativeFunctionsNameMap =  {
    { "print", -1 }
};

Variable callNativeFun(int index, std::vector<Variable> vars) {
    return nativeFunctions[-index - 1](vars);
}

NativeFun nativeFunctions[] = {
    print
};

Variable print(std::vector<Variable> vars) {
    if (vars[0].isInt()) {
        std::cout << vars[0].getInt() << std::endl;
    } else if(vars[0].isBool()) {
        std::cout << vars[0].getBool() << std::endl;
    } else if (vars[0].isNaN()) {
        std::cout << "NaN" << std::endl;
    } else {
        throwError("Unknown type in print");
    }
    return Variable();
}