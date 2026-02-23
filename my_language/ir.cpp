#include "ir.h"

int IR::generate(const std::unique_ptr<Parser::NodeAST>& node) {
    OP newInstruction;

    switch (node->token.type) {

        case Token::Type::Int:
            newInstruction.operation = OP::Type::Int;
            newInstruction.dst = getNextIndex();
            newInstruction.val = node->token.val;
            instructions.push_back(newInstruction);
            return newInstruction.dst;

        case Token::Type::Bool:
            newInstruction.operation = OP::Type::Bool;
            newInstruction.dst = getNextIndex();
            newInstruction.bval = node->token.bval;
            instructions.push_back(newInstruction);
            return newInstruction.dst;

        case Token::Type::Block:
            pushBlock();
            for (int i = 0; i < node->statements.size(); i++) {
                generate(node->statements[i]);
            }
            popBlock();
            return -1;

        case Token::Type::FBlock:
            for (int i = 0; i < node->statements.size(); i++) {
                generate(node->statements[i]);
            }
            return -1;

        case Token::Type::Neg:
            newInstruction.operation = OP::Type::Neg;
            newInstruction.src1 = generate(node->left);
            newInstruction.dst = getNextIndex();
            instructions.push_back(newInstruction);
            return newInstruction.dst;

        case Token::Type::Assign:
            newInstruction.operation = OP::Type::Assign;
            newInstruction.dst = generate(node->right);
            newInstruction.name = node->left->token.name;
            instructions.push_back(newInstruction);
            return -1;

        case Token::Type::Var:
            newInstruction.operation = OP::Type::Load;
            newInstruction.dst = getNextIndex();
            newInstruction.name = node->token.name;
            instructions.push_back(newInstruction);
            return newInstruction.dst;

        case Token::Type::Arg:
            newInstruction.operation = OP::Type::Assign;
            newInstruction.dst = getNextIndex();
            newInstruction.name = node->token.name;
            instructions.push_back(newInstruction);
            return newInstruction.dst;

        case Token::Type::If:
            addIfInstructions(node);
            return -1;

        case Token::Type::While:
            addWhileInstructions(node);
            return -1;

        case Token::Type::For:
            addForInstructions(node);
            return -1;

        case Token::Type::Dec:
            newInstruction.operation = OP::Type::Dec;
            newInstruction.name = node->left->token.name;
            instructions.push_back(newInstruction);
            if (node->right != nullptr) {
                OP assign;
                assign.operation = OP::Type::Assign;
                assign.dst = generate(node->right);
                assign.name = newInstruction.name;
                instructions.push_back(assign);
            }
            return -1;

        case Token::Type::And:
            return addAndOrInstructions(node, true);

        case Token::Type::Or:
            return addAndOrInstructions(node, false);

        case Token::Type::Fun: {
            currContext = ContextType::FunDeclaration;
            for (int i = 0; i < node->statements.size(); i++) {
                generate(node->statements[i]);
            }
            currContext = ContextType::Default;
            return -1;
        }

        case Token::Type::Ret:
            break;

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
    instructions.push_back(newInstruction);
    return newInstruction.dst;
}

void IR::addIfInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    OP JmpZ;
    JmpZ.operation = OP::Type::JmpZ;
    JmpZ.src1 = generate(node->condition);
    
    int pcSkipIf = instructions.size(); // Save Jmpnz location
    instructions.push_back(JmpZ);
    generate(node->left);
    instructions[pcSkipIf].dst = instructions.size();

    if (node->right) {
        OP Jmp;
        Jmp.operation = OP::Type::Jmp;
        int pcSkipElse = instructions.size(); // Save Jmp location
        instructions.push_back(Jmp);

        instructions[pcSkipIf].dst = instructions.size(); // Fix jmpnz location

        generate(node->right);
        instructions[pcSkipElse].dst = instructions.size();
    }
}

void IR::addWhileInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    int pcStart = instructions.size();
    OP JmpZ;
    JmpZ.operation = OP::Type::JmpZ;
    JmpZ.src1 = generate(node->condition);
    
    int pcEnd = instructions.size(); // Save Jmpnz location
    instructions.push_back(JmpZ);
    generate(node->left);

    OP Jmp;
    Jmp.operation = OP::Type::Jmp;
    Jmp.dst = pcStart;
    instructions.push_back(Jmp);

    instructions[pcEnd].dst = instructions.size();
}

void IR::addForInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    pushBlock();

    generate(node->right); // generate initialization

    int pcStart = instructions.size();
    OP JmpZ;
    JmpZ.operation = OP::Type::JmpZ;
    JmpZ.src1 = generate(node->condition);
    
    int pcEnd = instructions.size(); // Save Jmpnz location
    instructions.push_back(JmpZ);
    generate(node->left); // generate for loop block
    generate(node->increment); // generate incrementation

    OP Jmp;
    Jmp.operation = OP::Type::Jmp;
    Jmp.dst = pcStart;
    instructions.push_back(Jmp);

    instructions[pcEnd].dst = instructions.size();

    popBlock();
}

int IR::addAndOrInstructions(const std::unique_ptr<Parser::NodeAST>& node, bool ifAnd) {
    int left = generate(node->left);

    OP JmpLeft;
    JmpLeft.operation = ifAnd ? OP::Type::JmpZ : OP::Type::JmpNZ;
    JmpLeft.src1 = left;
    int pcJmpLeft = instructions.size(); // Save Jmpnz location
    instructions.push_back(JmpLeft);
    
    int right = generate(node->right);

    OP JmpRight;
    JmpRight.operation = ifAnd ? OP::Type::JmpZ : OP::Type::JmpNZ;
    JmpRight.src1 = right;
    int pcJmpRight = instructions.size(); // Save Jmpnz location
    instructions.push_back(JmpRight);

    int res = getNextIndex();
    addConst(res, ifAnd ? true : false);

    OP Jmp;
    Jmp.operation = OP::Type::Jmp;
    int pcJmp = instructions.size(); // Save Jmp location
    instructions.push_back(Jmp);

    instructions[pcJmpLeft].dst = instructions.size();
    instructions[pcJmpRight].dst = instructions.size();

    addConst(res, ifAnd ? false : true);

    instructions[pcJmp].dst = instructions.size();

    return res;
}

int IR::addConst(int dst, int val) {
    OP mov;
    mov.operation = OP::Type::Int;
    mov.dst = dst;
    mov.val = val;
    instructions.push_back(mov);
    return mov.dst;
}

int IR::addConst(int dst, bool val) {
    OP mov;
    mov.operation = OP::Type::Bool;
    mov.dst = dst;
    mov.bval = val;
    instructions.push_back(mov);
    return mov.dst;
}

void IR::pushBlock() {
    OP block;
    block.operation = OP::Type::Block;
    instructions.push_back(block);
}

void IR::popBlock() {
    OP deblock;
    deblock.operation = OP::Type::Deblock;
    instructions.push_back(deblock);    
}

void IR::print() {
    for (int i = 0; i < instructions.size(); i++) {
        std::cout << i << ". " << instructions[i];
    }
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
            cout << "Assign \"" << inst.name << "\", r" << inst.dst << std::endl;
            return cout;

        case IR::OP::Type::Load:
            cout << "Load r" << inst.dst << ", \"" << inst.name << "\"" << std::endl;
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
        
        case IR::OP::Type::Dec:
            cout << "Dec \"" << inst.name << "\"" << std::endl;
            return cout;
        
        case IR::OP::Type::Block:
            cout << "PushBlock" << std::endl;
            return cout;
        
        case IR::OP::Type::Deblock:
            cout << "PopBlock" << std::endl;
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