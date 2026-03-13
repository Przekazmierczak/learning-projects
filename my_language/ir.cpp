#include "ir.h"

int IR::generate(const std::unique_ptr<Parser::NodeAST>& node) {
    Opcode newInstruction;

    switch (node->token.type) {

        case Token::Type::Int:
            return addIntInstructions(node);

        case Token::Type::Bool:
            return addBoolInstructions(node);

        case Token::Type::String:
            return addStringInstructions(node);

        case Token::Type::Block:
            addBlockInstructions(node);
            return -1;

        case Token::Type::Neg:
            return addNegInstructions(node);

        case Token::Type::Assign:
            addAssignInstructions(node);
            return -1;

        case Token::Type::Var:
            return addVarInstructions(node);

        case Token::Type::If:
            addIfInstructions(node);
            return -1;

        case Token::Type::While:
            addWhileInstructions(node);
            return -1;

        case Token::Type::For:
            addForInstructions(node);
            return -1;

        case Token::Type::And:
            return addAndOrInstructions(node, true);

        case Token::Type::Or:
            return addAndOrInstructions(node, false);

        case Token::Type::Dec:
            addDecInstructions(node);
            return -1;

        case Token::Type::Fun:
            addFunInstructions(node);
            return -1;

        case Token::Type::Call:
            return addCallInstructions(node);

        case Token::Type::Ret:
            return addRetInstructions(node);

        case Token::Type::Add:newInstruction.operation = Opcode::Type::Add; break;
        case Token::Type::Sub: newInstruction.operation = Opcode::Type::Sub; break;
        case Token::Type::Mul: newInstruction.operation = Opcode::Type::Mul; break;
        case Token::Type::Div: newInstruction.operation = Opcode::Type::Div; break;
        case Token::Type::CmpEq: newInstruction.operation = Opcode::Type::CmpEq; break;
        case Token::Type::CmpNEq: newInstruction.operation = Opcode::Type::CmpNEq; break;
        case Token::Type::CmpGt: newInstruction.operation = Opcode::Type::CmpGt; break;
        case Token::Type::CmpLs: newInstruction.operation = Opcode::Type::CmpLs; break;
        case Token::Type::CmpGtEq: newInstruction.operation = Opcode::Type::CmpGtEq; break;
        case Token::Type::CmpLsEq: newInstruction.operation = Opcode::Type::CmpLsEq; break;

        default:
            throwError("Unsupported token in IR generation", node->token.line, node->token.position);
    }

    newInstruction.src1 = generate(node->left);
    newInstruction.src2 = generate(node->right);
    newInstruction.dst = getNextIndex();
    pushInstruction(newInstruction);
    return newInstruction.dst;
}

int IR::addIntInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    Opcode newInstruction;
    newInstruction.operation = Opcode::Type::Int;
    newInstruction.dst = getNextIndex();
    newInstruction.val = node->token.val;
    pushInstruction(newInstruction);
    return newInstruction.dst;
}

int IR::addBoolInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    Opcode newInstruction;
    newInstruction.operation = Opcode::Type::Bool;
    newInstruction.dst = getNextIndex();
    newInstruction.bval = node->token.bval;
    pushInstruction(newInstruction);
    return newInstruction.dst;
}

int IR::addStringInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    Opcode newInstruction;
    newInstruction.operation = Opcode::Type::String;
    newInstruction.dst = getNextIndex();
    newInstruction.name = node->token.strval;
    pushInstruction(newInstruction);
    return newInstruction.dst;
}

void IR::addBlockInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    currVarMap().emplace_back();

    for (int i = 0; i < node->statements.size(); i++) {
        generate(node->statements[i]);
    }

    currVarMap().pop_back();
}

int IR::addNegInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    Opcode newInstruction;
    newInstruction.operation = Opcode::Type::Neg;
    newInstruction.src1 = generate(node->left);
    newInstruction.dst = getNextIndex();
    pushInstruction(newInstruction);
    return newInstruction.dst;
}

void IR::addAssignInstructions(const std::unique_ptr<Parser::NodeAST>& node){
    int index = findInMaps(node->left->token.strval);
    if (index == -1) {
        throwError("Variable \"" + node->left->token.strval + "\" was never declared");
    }

    Opcode newInstruction;
    newInstruction.operation = Opcode::Type::Assign;
    newInstruction.dst = generate(node->right);
    newInstruction.src1 = currVarMap()[index][node->left->token.strval];
    pushInstruction(newInstruction);
}

int IR::addVarInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    int index = findInMaps(node->token.strval);
    if (index == -1) {
        throwError("Unknown variable name: \"" + node->token.strval + "\"");
    }

    Opcode newInstruction;
    newInstruction.operation = Opcode::Type::Load;
    newInstruction.dst = getNextIndex();
    newInstruction.src1 = currVarMap()[index][node->token.strval];
    pushInstruction(newInstruction);
    return newInstruction.dst;
}

void IR::addIfInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    Opcode JmpZ;
    JmpZ.operation = Opcode::Type::JmpZ;
    JmpZ.src1 = generate(node->condition);
    
    int pcSkipIf = currInstructionArray().size(); // Save Jmpnz location
    pushInstruction(JmpZ);
    generate(node->left);
    currInstructionArray()[pcSkipIf].dst = currInstructionArray().size();

    if (node->right) {
        Opcode Jmp;
        Jmp.operation = Opcode::Type::Jmp;
        int pcSkipElse = currInstructionArray().size(); // Save Jmp location
        pushInstruction(Jmp);

        currInstructionArray()[pcSkipIf].dst = currInstructionArray().size(); // Fix jmpnz location

        generate(node->right);
        currInstructionArray()[pcSkipElse].dst = currInstructionArray().size();
    }
}

void IR::addWhileInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    int pcStart = currInstructionArray().size();
    Opcode JmpZ;
    JmpZ.operation = Opcode::Type::JmpZ;
    JmpZ.src1 = generate(node->condition);
    
    int pcEnd = currInstructionArray().size(); // Save Jmpnz location
    pushInstruction(JmpZ);
    generate(node->left);

    Opcode Jmp;
    Jmp.operation = Opcode::Type::Jmp;
    Jmp.dst = pcStart;
    pushInstruction(Jmp);

    currInstructionArray()[pcEnd].dst = currInstructionArray().size();
}

void IR::addForInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    generate(node->right); // generate initialization

    int pcStart = currInstructionArray().size();
    Opcode JmpZ;
    JmpZ.operation = Opcode::Type::JmpZ;
    JmpZ.src1 = generate(node->condition);
    
    int pcEnd = currInstructionArray().size(); // Save Jmpnz location
    pushInstruction(JmpZ);
    generate(node->left); // generate for loop block
    generate(node->increment); // generate incrementation

    Opcode Jmp;
    Jmp.operation = Opcode::Type::Jmp;
    Jmp.dst = pcStart;
    pushInstruction(Jmp);

    currInstructionArray()[pcEnd].dst = currInstructionArray().size();
}

int IR::addAndOrInstructions(const std::unique_ptr<Parser::NodeAST>& node, bool ifAnd) {
    int left = generate(node->left);

    Opcode JmpLeft;
    JmpLeft.operation = ifAnd ? Opcode::Type::JmpZ : Opcode::Type::JmpNZ;
    JmpLeft.src1 = left;
    int pcJmpLeft = currInstructionArray().size(); // Save Jmpnz location
    pushInstruction(JmpLeft);
    
    int right = generate(node->right);

    Opcode JmpRight;
    JmpRight.operation = ifAnd ? Opcode::Type::JmpZ : Opcode::Type::JmpNZ;
    JmpRight.src1 = right;
    int pcJmpRight = currInstructionArray().size(); // Save Jmpnz location
    pushInstruction(JmpRight);

    int res = getNextIndex();
    addConst(res, ifAnd ? true : false);

    Opcode Jmp;
    Jmp.operation = Opcode::Type::Jmp;
    int pcJmp = currInstructionArray().size(); // Save Jmp location
    pushInstruction(Jmp);

    currInstructionArray()[pcJmpLeft].dst = currInstructionArray().size();
    currInstructionArray()[pcJmpRight].dst = currInstructionArray().size();

    addConst(res, ifAnd ? false : true);

    currInstructionArray()[pcJmp].dst = currInstructionArray().size();

    return res;
}

void IR::addDecInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    if (findInMap(currVarMap().size() - 1, node->left->token.strval)) {
        throwError("Variable \"" + node->left->token.strval + "\" is already declared");
    }

    int newIndex = getNextVarIndex();
    currVarMap().back()[node->left->token.strval] = newIndex;

    if (node->right != nullptr) {
        Opcode assign;
        assign.operation = Opcode::Type::Assign;
        assign.dst = generate(node->right);
        assign.src1 = newIndex;
        pushInstruction(assign);
    }
}

void IR::addFunInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    currContext = ContextType::FunDeclaration;
    currLocalReg = 0;
    nextFunctionsVarIndex = 0;
    functionsVarMap.clear();
    functionsVarMap.emplace_back();

    std::string name = node->token.strval;
    int startPC = functionsInstructions.size();
    int argsCount = node->statements.size();

    addFunctionMeta(name, FunctionMeta(startPC, argsCount, 0, false));

    for (int i = 0; i < node->statements.size(); i++) {
        currVarMap().back()[node->statements[i]->token.strval] = getNextVarIndex();
    }

    generate(node->left);

    Opcode blankReturn;
    blankReturn.operation = Opcode::Type::Ret;
    blankReturn.dst = getNextIndex();
    pushInstruction(blankReturn);

    functionsMetaMap[functionsNameMap[name]].regCount = currLocalReg - argsCount;
    functionsMetaMap[functionsNameMap[name]].varCount = nextFunctionsVarIndex;

    currContext = ContextType::Default;
}

int IR::addCallInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    std::string name = node->token.strval;
    int functionIndex;
    if (functionsNameMap.find(name) != functionsNameMap.end()) {
        if (functionsMetaMap[functionsNameMap[name]].argsCount != node->statements.size()) {
            throwError("Incorrect number of arguments in \"" + name + "\"", node->token.line, node->token.position);
        }
    } else {
        throwError("Function \"" + name + "\" was never declared", node->token.line, node->token.position);
    }

    int ret = getNextIndex();

    Opcode loadArg;
    loadArg.operation = Opcode::Type::Push;
    for (int i = 0; i < node->statements.size(); i++) {
        loadArg.src1 = generate(node->statements[i]);
        pushInstruction(loadArg);
    }

    Opcode callInstruction;
    callInstruction.operation = Opcode::Type::Call;
    callInstruction.src1 = functionsNameMap[name];
    callInstruction.dst = ret;
    pushInstruction(callInstruction);

    return callInstruction.dst;
}

int IR::addRetInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    Opcode newInstruction;
    newInstruction.operation = Opcode::Type::Ret;
    newInstruction.dst = generate(node->left);
    pushInstruction(newInstruction);
    return newInstruction.dst;
}

int IR::addConst(int dst, int val) {
    Opcode mov;
    mov.operation = Opcode::Type::Int;
    mov.dst = dst;
    mov.val = val;
    pushInstruction(mov);
    return mov.dst;
}

int IR::addConst(int dst, bool val) {
    Opcode mov;
    mov.operation = Opcode::Type::Bool;
    mov.dst = dst;
    mov.bval = val;
    pushInstruction(mov);
    return mov.dst;
}

void IR::pushInstruction(Opcode instruction) {
    if (currContext == ContextType::Default) {
        instructions.push_back(instruction);
    } else if (currContext == ContextType::FunDeclaration) {
        functionsInstructions.push_back(instruction);
    }
}

std::vector<IR::Opcode>& IR::currInstructionArray() {
    if (currContext == ContextType::Default) {
        return instructions;
    } else if (currContext == ContextType::FunDeclaration) {
        return functionsInstructions;
    }
    throwError("Incorrect currContext");
}

std::vector<std::unordered_map<std::string, int>>& IR::currVarMap() {
    if (currContext == ContextType::Default) {
        return varMap;
    } else if (currContext == ContextType::FunDeclaration) {
        return functionsVarMap;
    }
    throwError("Incorrect currContext");
}

int IR::getNextVarIndex() {
    if (currContext == ContextType::Default) {
        return nextVarIndex++;
    } else if (currContext == ContextType::FunDeclaration) {
        return nextFunctionsVarIndex++;
    }
    throwError("Incorrect currContext");
}

bool IR::findInMap(int index, const std::string& name) {
    return currVarMap()[index].find(name) != currVarMap()[index].end();
}

int IR::findInMaps(const std::string& name) {
    for (int i = currVarMap().size() - 1; i >= 0; i--) {
        if (findInMap(i, name)) return i;
    }
    return -1;
}

void IR::addFunctionMeta(std::string name, FunctionMeta functionMeta) {
    if (functionsNameMap.find(name) != functionsNameMap.end()) {
        throwError("Function \"" + name + "\" is already declared");
    } else {
        functionsNameMap[name] = functionsNameMap.size();
        functionsMetaMap.push_back(functionMeta);
    }
}

void IR::print() {
    std::cout << "Global instructions:" << std::endl;
    for (int i = 0; i < instructions.size(); i++) {
        std::cout << i << ". " << instructions[i];
    }
    std::cout << std::endl;

    std::cout << "Function instructions:" << std::endl;
    for (int i = 0; i < functionsInstructions.size(); i++) {
        std::cout << i << ". " << functionsInstructions[i];
    }
    std::cout << std::endl;

    std::cout << "Functions metadata:" << std::endl;
    for (int i = 0; i < functionsMetaMap.size(); i++)
    {
        std::cout << i << " : startPC " << functionsMetaMap[i].startPC
                  << ", argsCount " << functionsMetaMap[i].argsCount
                  << ", regCount " << functionsMetaMap[i].regCount
                  << ", varCount " << functionsMetaMap[i].varCount
                  << ", native " << functionsMetaMap[i].native
                  << std::endl;
    }
    std::cout << std::endl;
}

std::ostream& operator << (std::ostream& cout, IR::Opcode& inst)
{
    std::string op;
    switch (inst.operation) {

        case IR::Opcode::Type::Int:
            cout << "Move r" << inst.dst << ", " << inst.val << std::endl;
            return cout;

        case IR::Opcode::Type::Bool:
            cout << "Move r" << inst.dst << ", " << inst.bval << std::endl;
            return cout;

        case IR::Opcode::Type::String:
            cout << "Move r" << inst.dst << ", \"" << inst.name << "\"" << std::endl;
            return cout;

        case IR::Opcode::Type::Neg:
            cout << "Neg" << " r" << inst.dst << ", r" << inst.src1 << std::endl;
            return cout;

        case IR::Opcode::Type::Assign:
            cout << "Assign v" << inst.src1 << ", r" << inst.dst << std::endl;
            return cout;

        case IR::Opcode::Type::Load:
            cout << "Load r" << inst.dst << ", v" << inst.src1 << std::endl;
            return cout;

        case IR::Opcode::Type::Push:
            cout << "Push r" << inst.src1 << std::endl;
            return cout;

        case IR::Opcode::Type::Jmp:
            cout << "Jmp pc" << inst.dst << std::endl;
            return cout;

        case IR::Opcode::Type::JmpZ:
            cout << "JmpZ pc" << inst.dst << ", r" << inst.src1 << std::endl;
            return cout;

        case IR::Opcode::Type::JmpNZ:
            cout << "JmpNZ pc" << inst.dst << ", r" << inst.src1 << std::endl;
            return cout;

        case IR::Opcode::Type::Call:
            cout << "Call r" << inst.dst << ", f" << inst.src1 << std::endl;
            return cout;

        case IR::Opcode::Type::Ret:
            cout << "Return r" << inst.dst << std::endl;
            return cout;

        case IR::Opcode::Type::Add: op = "Add"; break;
        case IR::Opcode::Type::Sub: op = "Sub"; break;
        case IR::Opcode::Type::Mul: op = "Mul"; break;
        case IR::Opcode::Type::Div: op = "Div"; break;
        case IR::Opcode::Type::CmpEq: op = "CmpEq"; break;
        case IR::Opcode::Type::CmpNEq: op = "CmpNEq"; break;
        case IR::Opcode::Type::CmpGt: op = "CmpGt"; break;
        case IR::Opcode::Type::CmpLs: op = "CmpLs"; break;
        case IR::Opcode::Type::CmpGtEq: op = "CmpGtEq"; break;
        case IR::Opcode::Type::CmpLsEq: op = "CmpLsEq"; break;

        default:
            throwError("Unsupported token in << overload");
    }
    cout << op << " r" << inst.dst << ", r" << inst.src1 << ", r" << inst.src2 << std::endl;
    return cout;
}