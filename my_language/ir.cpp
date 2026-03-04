#include "ir.h"

int IR::generate(const std::unique_ptr<Parser::NodeAST>& node) {
    OP newInstruction;

    switch (node->token.type) {

        case Token::Type::Int:
            return addIntInstructions(node);

        case Token::Type::Bool:
            return addBoolInstructions(node);

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

        case Token::Type::Add:newInstruction.operation = OP::Type::Add; break;
        case Token::Type::Sub: newInstruction.operation = OP::Type::Sub; break;
        case Token::Type::Mul: newInstruction.operation = OP::Type::Mul; break;
        case Token::Type::Div: newInstruction.operation = OP::Type::Div; break;
        case Token::Type::CmpEq: newInstruction.operation = OP::Type::CmpEq; break;
        case Token::Type::CmpNEq: newInstruction.operation = OP::Type::CmpNEq; break;
        case Token::Type::CmpGt: newInstruction.operation = OP::Type::CmpGt; break;
        case Token::Type::CmpLs: newInstruction.operation = OP::Type::CmpLs; break;
        case Token::Type::CmpGtEq: newInstruction.operation = OP::Type::CmpGtEq; break;
        case Token::Type::CmpLsEq: newInstruction.operation = OP::Type::CmpLsEq; break;

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
    OP newInstruction;
    newInstruction.operation = OP::Type::Int;
    newInstruction.dst = getNextIndex();
    newInstruction.val = node->token.val;
    pushInstruction(newInstruction);
    return newInstruction.dst;
}

int IR::addBoolInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    OP newInstruction;
    newInstruction.operation = OP::Type::Bool;
    newInstruction.dst = getNextIndex();
    newInstruction.bval = node->token.bval;
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
    OP newInstruction;
    newInstruction.operation = OP::Type::Neg;
    newInstruction.src1 = generate(node->left);
    newInstruction.dst = getNextIndex();
    pushInstruction(newInstruction);
    return newInstruction.dst;
}

void IR::addAssignInstructions(const std::unique_ptr<Parser::NodeAST>& node){
    int index = findInMaps(node->left->token.name);
    if (index == -1) {
        throwError("Variable \"" + node->left->token.name + "\" was never declared");
    }

    OP newInstruction;
    newInstruction.operation = OP::Type::Assign;
    newInstruction.dst = generate(node->right);
    newInstruction.src1 = currVarMap()[index][node->left->token.name];
    pushInstruction(newInstruction);
}

int IR::addVarInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    int index = findInMaps(node->token.name);
    if (index == -1) {
        throwError("Unknown variable name: \"" + node->token.name + "\"");
    }

    OP newInstruction;
    newInstruction.operation = OP::Type::Load;
    newInstruction.dst = getNextIndex();
    newInstruction.src1 = currVarMap()[index][node->token.name];
    pushInstruction(newInstruction);
    return newInstruction.dst;
}

void IR::addIfInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    OP JmpZ;
    JmpZ.operation = OP::Type::JmpZ;
    JmpZ.src1 = generate(node->condition);
    
    int pcSkipIf = currInstructionArray().size(); // Save Jmpnz location
    pushInstruction(JmpZ);
    generate(node->left);
    currInstructionArray()[pcSkipIf].dst = currInstructionArray().size();

    if (node->right) {
        OP Jmp;
        Jmp.operation = OP::Type::Jmp;
        int pcSkipElse = currInstructionArray().size(); // Save Jmp location
        pushInstruction(Jmp);

        currInstructionArray()[pcSkipIf].dst = currInstructionArray().size(); // Fix jmpnz location

        generate(node->right);
        currInstructionArray()[pcSkipElse].dst = currInstructionArray().size();
    }
}

void IR::addWhileInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    int pcStart = currInstructionArray().size();
    OP JmpZ;
    JmpZ.operation = OP::Type::JmpZ;
    JmpZ.src1 = generate(node->condition);
    
    int pcEnd = currInstructionArray().size(); // Save Jmpnz location
    pushInstruction(JmpZ);
    generate(node->left);

    OP Jmp;
    Jmp.operation = OP::Type::Jmp;
    Jmp.dst = pcStart;
    pushInstruction(Jmp);

    currInstructionArray()[pcEnd].dst = currInstructionArray().size();
}

void IR::addForInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    generate(node->right); // generate initialization

    int pcStart = currInstructionArray().size();
    OP JmpZ;
    JmpZ.operation = OP::Type::JmpZ;
    JmpZ.src1 = generate(node->condition);
    
    int pcEnd = currInstructionArray().size(); // Save Jmpnz location
    pushInstruction(JmpZ);
    generate(node->left); // generate for loop block
    generate(node->increment); // generate incrementation

    OP Jmp;
    Jmp.operation = OP::Type::Jmp;
    Jmp.dst = pcStart;
    pushInstruction(Jmp);

    currInstructionArray()[pcEnd].dst = currInstructionArray().size();
}

int IR::addAndOrInstructions(const std::unique_ptr<Parser::NodeAST>& node, bool ifAnd) {
    int left = generate(node->left);

    OP JmpLeft;
    JmpLeft.operation = ifAnd ? OP::Type::JmpZ : OP::Type::JmpNZ;
    JmpLeft.src1 = left;
    int pcJmpLeft = currInstructionArray().size(); // Save Jmpnz location
    pushInstruction(JmpLeft);
    
    int right = generate(node->right);

    OP JmpRight;
    JmpRight.operation = ifAnd ? OP::Type::JmpZ : OP::Type::JmpNZ;
    JmpRight.src1 = right;
    int pcJmpRight = currInstructionArray().size(); // Save Jmpnz location
    pushInstruction(JmpRight);

    int res = getNextIndex();
    addConst(res, ifAnd ? true : false);

    OP Jmp;
    Jmp.operation = OP::Type::Jmp;
    int pcJmp = currInstructionArray().size(); // Save Jmp location
    pushInstruction(Jmp);

    currInstructionArray()[pcJmpLeft].dst = currInstructionArray().size();
    currInstructionArray()[pcJmpRight].dst = currInstructionArray().size();

    addConst(res, ifAnd ? false : true);

    currInstructionArray()[pcJmp].dst = currInstructionArray().size();

    return res;
}

void IR::addDecInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    if (findInMap(currVarMap().size() - 1, node->left->token.name)) {
        throwError("Variable \"" + node->left->token.name + "\" is already declared");
    }

    int newIndex = getNextVarIndex();
    currVarMap().back()[node->left->token.name] = newIndex;

    if (node->right != nullptr) {
        OP assign;
        assign.operation = OP::Type::Assign;
        assign.dst = generate(node->right);
        assign.src1 = newIndex;
        pushInstruction(assign);
    }
}

void IR::addFunInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    currContext = ContextType::FunDeclaration;
    currLocalReg = 0;
    nextFunctionVarIndex = 0;
    functionVarMap.clear();
    functionVarMap.emplace_back();

    std::string name = node->token.name;
    int startPC = functionsInstructions.size();
    int argsCount = node->statements.size();

    addFunctionMeta(name, FunctionMeta(startPC, argsCount, 0));

    for (int i = 0; i < node->statements.size(); i++) {
        currVarMap().back()[node->statements[i]->token.name] = getNextVarIndex();
    }

    generate(node->left);

    OP blankReturn;
    blankReturn.operation = OP::Type::Ret;
    blankReturn.dst = getNextIndex();
    pushInstruction(blankReturn);

    functionsMap[name].regCount = currLocalReg - argsCount;
    functionsMap[name].varCount = nextFunctionVarIndex;

    currContext = ContextType::Default;
}

int IR::addCallInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    std::string name = node->token.name;

    if (functionsMap.find(name) != functionsMap.end()) {
        if (functionsMap[name].argsCount != node->statements.size()) {
            throwError("Incorrect number of arguments in \"" + name + "\"", node->token.line, node->token.position);
        }
    } else {
        throwError("Function \"" + name + "\" was never declared", node->token.line, node->token.position);
    }

    int ret = getNextIndex();

    OP loadArg;
    loadArg.operation = OP::Type::Push;
    for (int i = 0; i < node->statements.size(); i++) {
        loadArg.src1 = generate(node->statements[i]);
        pushInstruction(loadArg);
    }

    OP callInstruction;
    callInstruction.operation = OP::Type::Call;
    callInstruction.name = name;
    callInstruction.dst = ret;
    pushInstruction(callInstruction);

    return callInstruction.dst;
}

int IR::addRetInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    OP newInstruction;
    newInstruction.operation = OP::Type::Ret;
    newInstruction.dst = generate(node->left);
    pushInstruction(newInstruction);
    return newInstruction.dst;
}

int IR::addConst(int dst, int val) {
    OP mov;
    mov.operation = OP::Type::Int;
    mov.dst = dst;
    mov.val = val;
    pushInstruction(mov);
    return mov.dst;
}

int IR::addConst(int dst, bool val) {
    OP mov;
    mov.operation = OP::Type::Bool;
    mov.dst = dst;
    mov.bval = val;
    pushInstruction(mov);
    return mov.dst;
}

void IR::pushInstruction(OP instruction) {
    if (currContext == ContextType::Default) {
        instructions.push_back(instruction);
    } else if (currContext == ContextType::FunDeclaration) {
        functionsInstructions.push_back(instruction);
    }
}

std::vector<IR::OP>& IR::currInstructionArray() {
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
        return functionVarMap;
    }
    throwError("Incorrect currContext");
}

int IR::getNextVarIndex() {
    if (currContext == ContextType::Default) {
        return nextVarIndex++;
    } else if (currContext == ContextType::FunDeclaration) {
        return nextFunctionVarIndex++;
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
    if (functionsMap.find(name) != functionsMap.end()) {
        throwError("Function \"" + name + "\" is already declared");
    } else {
        functionsMap[name] = functionMeta;
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
    for (const auto& pair : functionsMap)
    {
        std::cout << pair.first << " : startPC " << pair.second.startPC
                  << ", argsCount " << pair.second.argsCount
                  << ", regCount " << pair.second.regCount
                  << ", varCount " << pair.second.varCount
                  << std::endl;
    }
    std::cout << std::endl;
}

std::ostream& operator << (std::ostream& cout, IR::OP& inst)
{
    std::string op;
    switch (inst.operation) {

        case IR::OP::Type::Int:
            cout << "Move r" << inst.dst << ", " << inst.val << std::endl;
            return cout;

        case IR::OP::Type::Bool:
            cout << "Move r" << inst.dst << ", " << inst.bval << std::endl;
            return cout;

        case IR::OP::Type::Neg:
            cout << "Neg" << " r" << inst.dst << ", r" << inst.src1 << std::endl;
            return cout;

        case IR::OP::Type::Assign:
            cout << "Assign v" << inst.src1 << ", r" << inst.dst << std::endl;
            return cout;

        case IR::OP::Type::Load:
            cout << "Load r" << inst.dst << ", v" << inst.src1 << std::endl;
            return cout;

        case IR::OP::Type::Push:
            cout << "Push r" << inst.src1 << std::endl;
            return cout;

        case IR::OP::Type::Jmp:
            cout << "Jmp pc" << inst.dst << std::endl;
            return cout;

        case IR::OP::Type::JmpZ:
            cout << "JmpZ pc" << inst.dst << ", r" << inst.src1 << std::endl;
            return cout;

        case IR::OP::Type::JmpNZ:
            cout << "JmpNZ pc" << inst.dst << ", r" << inst.src1 << std::endl;
            return cout;

        case IR::OP::Type::Call:
            cout << "Call r" << inst.dst << ", \"" << inst.name << "\"" << std::endl;
            return cout;

        case IR::OP::Type::Ret:
            cout << "Return r" << inst.dst << std::endl;
            return cout;

        case IR::OP::Type::Add: op = "Add"; break;
        case IR::OP::Type::Sub: op = "Sub"; break;
        case IR::OP::Type::Mul: op = "Mul"; break;
        case IR::OP::Type::Div: op = "Div"; break;
        case IR::OP::Type::CmpEq: op = "CmpEq"; break;
        case IR::OP::Type::CmpNEq: op = "CmpNEq"; break;
        case IR::OP::Type::CmpGt: op = "CmpGt"; break;
        case IR::OP::Type::CmpLs: op = "CmpLs"; break;
        case IR::OP::Type::CmpGtEq: op = "CmpGtEq"; break;
        case IR::OP::Type::CmpLsEq: op = "CmpLsEq"; break;
        case IR::OP::Type::And: op = "And"; break;
        case IR::OP::Type::Or: op = "Or"; break;

        default:
            throwError("Unsupported token in << overload");
    }
    cout << op << " r" << inst.dst << ", r" << inst.src1 << ", r" << inst.src2 << std::endl;
    return cout;
}