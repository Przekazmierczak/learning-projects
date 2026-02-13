#include "ir.h"

int IR::generate(const std::unique_ptr<Parser::NodeAST>& node) {
    OP newInstruction;
    if (node->token.type == Token::Type::Int) {
        newInstruction.operation = OP::Type::Const;
        newInstruction.dst = index.getNext();
        newInstruction.val = node->token.val;
        instructions.push_back(newInstruction);
    } else if (node->token.type == Token::Type::Block) {
        for (int i = 0; i < node->statements.size(); i++) {
            generate(node->statements[i]);
        }
        return instructions.size();
    } else if (node->token.type == Token::Type::Neg) {
        newInstruction.operation = OP::Type::Neg;
        newInstruction.src1 = generate(node->left);
        newInstruction.dst = index.getNext();
        instructions.push_back(newInstruction);
    } else if (node->token.type == Token::Type::Assign) {
        newInstruction.operation = OP::Type::Assign;
        newInstruction.dst = generate(node->right);
        newInstruction.name = node->left->token.name;
        instructions.push_back(newInstruction);
        return -1;
    } else if (node->token.type == Token::Type::Var) {
        newInstruction.operation = OP::Type::Load;
        newInstruction.dst = index.getNext();
        newInstruction.name = node->token.name;
        instructions.push_back(newInstruction);
    } else if (node->token.type == Token::Type::If) {
        addIfInstructions(node);
        return -1;
    } else {
        if (node->token.type == Token::Type::Add) newInstruction.operation = OP::Type::Add;
        else if (node->token.type == Token::Type::Sub) newInstruction.operation = OP::Type::Sub;
        else if (node->token.type == Token::Type::Mul) newInstruction.operation = OP::Type::Mul;
        else if (node->token.type == Token::Type::Div) newInstruction.operation = OP::Type::Div;
        newInstruction.src1 = generate(node->left);
        newInstruction.src2 = generate(node->right);
        newInstruction.dst = index.getNext();
        instructions.push_back(newInstruction);
    }
    return newInstruction.dst;
}

void IR::addIfInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
    OP cmp;
    cmp.operation = OP::Type::Cmp;
    cmp.src1 = generate(node->condition);
    cmp.src2 = addConst(0);;
    cmp.dst = index.getNext();
    instructions.push_back(cmp);

    OP JmpZ;
    JmpZ.operation = OP::Type::JmpZ;
    JmpZ.src1 = cmp.dst;
    
    int pc = instructions.size(); // Save Jmpz location
    instructions.push_back(JmpZ);
    instructions[pc].dst = generate(node->left);

    if (node->right) {
        instructions[pc].dst++; // Pass over new Jmp instruction

        OP Jmp;
        Jmp.operation = OP::Type::Jmp;
        pc = instructions.size();
        instructions.push_back(Jmp);
        instructions[pc].dst = generate(node->right);
    }
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
    } else {
        std::string op;
        if (inst.operation == IR::OP::Type::Add) op = "Add";
        else if (inst.operation == IR::OP::Type::Sub) op = "Sub";
        else if (inst.operation == IR::OP::Type::Mul) op = "Mul";
        else if (inst.operation == IR::OP::Type::Div) op = "Div";
        else if (inst.operation == IR::OP::Type::Cmp) op = "Cmp";
        cout << op << " r" << inst.dst << ", r" << inst.src1 << ", r" << inst.src2 << std::endl;
    }
    return cout;
}