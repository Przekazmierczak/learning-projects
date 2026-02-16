#include "ir.h"

int IR::generate(const std::unique_ptr<Parser::NodeAST>& node) {
    OP newInstruction;

    switch (node->token.type) {
        
        case Token::Type::Int:
            newInstruction.operation = OP::Type::Const;
            newInstruction.dst = index.getNext();
            newInstruction.val = node->token.val;
            instructions.push_back(newInstruction);
            return newInstruction.dst;

        case Token::Type::Block:
            for (int i = 0; i < node->statements.size(); i++) {
                generate(node->statements[i]);
            }
            return -1;

        case Token::Type::Neg:
            newInstruction.operation = OP::Type::Neg;
            newInstruction.src1 = generate(node->left);
            newInstruction.dst = index.getNext();
            instructions.push_back(newInstruction);
            return newInstruction.dst;

        case Token::Type::Assign:
            newInstruction.operation = OP::Type::Assign;
            newInstruction.dst = generate(node->right);
            newInstruction.name = node->left->token.name;
            instructions.push_back(newInstruction);
            return newInstruction.dst;

        case Token::Type::Var:
            newInstruction.operation = OP::Type::Load;
            newInstruction.dst = index.getNext();
            newInstruction.name = node->token.name;
            instructions.push_back(newInstruction);
            return newInstruction.dst;

        case Token::Type::If:
            addIfInstructions(node);
            return -1;

        case Token::Type::While:
            addWhileInstructions(node);
            return -1;

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
    newInstruction.dst = index.getNext();
    instructions.push_back(newInstruction);
    return newInstruction.dst;
}

void IR::addIfInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    OP cmp;
    cmp.operation = OP::Type::CmpNEq;
    cmp.src1 = generate(node->condition);
    cmp.src2 = addConst(0);;
    cmp.dst = index.getNext();
    instructions.push_back(cmp);

    OP JmpZ;
    JmpZ.operation = OP::Type::JmpZ;
    JmpZ.src1 = cmp.dst;
    
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
    OP cmp;
    cmp.operation = OP::Type::CmpNEq;
    cmp.src1 = generate(node->condition);
    cmp.src2 = addConst(0);;
    cmp.dst = index.getNext();
    instructions.push_back(cmp);

    OP JmpZ;
    JmpZ.operation = OP::Type::JmpZ;
    JmpZ.src1 = cmp.dst;
    
    int pcEnd = instructions.size(); // Save Jmpnz location
    instructions.push_back(JmpZ);
    generate(node->left);

    OP Jmp;
    Jmp.operation = OP::Type::Jmp;
    Jmp.dst = pcStart;
    instructions.push_back(Jmp);

    instructions[pcEnd].dst = instructions.size();
}

int IR::addConst(int val) {
    OP mov;
    mov.operation = OP::Type::Const;
    mov.dst = index.getNext();
    mov.val = val;
    instructions.push_back(mov);
    return mov.dst;
}

void IR::print() {
    for (int i = 0; i < instructions.size(); i++) {
        std::cout << i << ". " << instructions[i];
    }
}

std::ostream& operator << (std::ostream& cout, IR::OP& inst)
{
    if (inst.operation == IR::OP::Type::Const) {
        cout << "Move r" << inst.dst << ", " << inst.val << std::endl;
    } else if (inst.operation == IR::OP::Type::Neg) {
        cout << "Neg" << " r" << inst.dst << ", r" << inst.src1 << std::endl;
    } else if (inst.operation == IR::OP::Type::Assign) {
        cout << "Assign \"" << inst.name << "\", r" << inst.dst << std::endl;
    } else if (inst.operation == IR::OP::Type::Load) {
        cout << "Load r" << inst.dst << ", \"" << inst.name << "\"" << std::endl;
    } else if (inst.operation == IR::OP::Type::Jmp) {
        cout << "Jmp pc" << inst.dst << std::endl;
    } else if (inst.operation == IR::OP::Type::JmpZ) {
        cout << "JmpZ pc" << inst.dst << ", r" << inst.src1 << std::endl;
    } else if (inst.operation == IR::OP::Type::JmpNZ) {
        cout << "JmpNZ pc" << inst.dst << ", r" << inst.src1 << std::endl;
    } else {
        std::string op;
        if (inst.operation == IR::OP::Type::Add) op = "Add";
        else if (inst.operation == IR::OP::Type::Sub) op = "Sub";
        else if (inst.operation == IR::OP::Type::Mul) op = "Mul";
        else if (inst.operation == IR::OP::Type::Div) op = "Div";
        else if (inst.operation == IR::OP::Type::CmpEq) op = "CmpEq";
        else if (inst.operation == IR::OP::Type::CmpNEq) op = "CmpNEq";
        else if (inst.operation == IR::OP::Type::CmpGt) op = "CmpGt";
        else if (inst.operation == IR::OP::Type::CmpLs) op = "CmpLs";
        else if (inst.operation == IR::OP::Type::CmpGtEq) op = "CmpGtEq";
        else if (inst.operation == IR::OP::Type::CmpLsEq) op = "CmpLsEq";
        cout << op << " r" << inst.dst << ", r" << inst.src1 << ", r" << inst.src2 << std::endl;
    }
    return cout;
}