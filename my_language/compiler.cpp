#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stack>
#include <memory>
#include <iostream>

struct Source {
    std::vector<std::string> source;
    std::vector<int> line;
    std::vector<int> position;

    void print() {
        for (int i = 0; i < source.size(); i++) {
            std::cout << "(" << source[i] << "[" << line[i] << ", " << position[i] << "]" << "), ";
        }
        std::cout << std::endl;
    }
};

struct Token {
    enum class Type {
        Int,
        Add,
        Sub,
        Mul,
        Div,
        Lpar,
        Rpar,
        Var
    };

    Type type;
    bool operant = false;
    int precedence = 0;
    int val = 0;
    std::string name = "";
    int line;
    int position;
};

Source lex(std::string code) {
    Source result;
    std::ifstream file(code);
    std::string line;
    std::string current;

    int currLine = 1;
    while (std::getline(file, line)) {
        int currPosition = 1;
        for (char c : line) {
            if (c == ' ') {
                if (!current.empty()) {
                    result.source.push_back(current);
                    result.line.push_back(currLine);
                    result.position.push_back(currPosition);
                    currPosition += current.size() + 1;
                    current.clear();
                }
            } else {
                current += c;
            }
        }

        if (!current.empty()) {
            result.source.push_back(current);
            result.line.push_back(currLine);
            result.position.push_back(currPosition);
        }
        current.clear();
        currLine++;
    }
    result.print();
    return result;
}

std::vector<Token> parse(Source source) {
    std::vector<Token> tokens;
    for (int i = 0; i < source.source.size(); i++) {
        Token current;
        if (source.source[i] == "+") {
            current.type = Token::Type::Add;
            current.operant = true;
            current.precedence = 20;
            current.line = source.line[i];
            current.position = source.position[i];
        } else if (source.source[i] == "-") {
            current.type = Token::Type::Sub;
            current.operant = true;
            current.precedence = 20;
            current.line = source.line[i];
            current.position = source.position[i];
        } else if (source.source[i] == "*") {
            current.type = Token::Type::Mul;
            current.operant = true;
            current.precedence = 30;
            current.line = source.line[i];
            current.position = source.position[i];
        } else if (source.source[i] == "/") {
            current.type = Token::Type::Div;
            current.operant = true;
            current.precedence = 30;
            current.line = source.line[i];
            current.position = source.position[i];
        } else if (source.source[i] == "(") {
            current.type = Token::Type::Lpar;
            current.operant = true;
            current.precedence = 40;
            current.line = source.line[i];
            current.position = source.position[i];
        } else if (source.source[i] == ")") {
            current.type = Token::Type::Rpar;
            current.operant = true;
            current.precedence = 10;
            current.line = source.line[i];
            current.position = source.position[i];
        } else if (std::isdigit(source.source[i][0]) || source.source[i][0] == '-') {
            current.type = Token::Type::Int;
            current.val = std::stoi(source.source[i]);
            current.line = source.line[i];
            current.position = source.position[i];
        } else {
            current.type = Token::Type::Var;
            current.name = source.source[i];
            current.line = source.line[i];
            current.position = source.position[i];
        }
        tokens.push_back(current);
    }
    return tokens;
}

struct NodeAST {
    bool block;
    Token token;
    std::unique_ptr<NodeAST> left;
    std::unique_ptr<NodeAST> right;
    std::vector<std::unique_ptr<NodeAST>> statements;

    NodeAST(Token newToken) : token(newToken) {}
};

void reduce(std::stack<std::unique_ptr<NodeAST>>& stackValues, std::stack<Token>& stackOperants) {
    std::unique_ptr<NodeAST> current = std::make_unique<NodeAST>(stackOperants.top());
    stackOperants.pop();

    current->right = std::move(stackValues.top());
    stackValues.pop();
    current->left = std::move(stackValues.top());
    stackValues.pop();

    stackValues.push(std::move(current));
}

std::unique_ptr<NodeAST>createAST (std::vector<Token> tokens) {
    std::stack<std::unique_ptr<NodeAST>> stackValues;
    std::stack<Token> stackOperants;

    for (int i = 0; i < tokens.size(); i++) {
        if (!tokens[i].operant) {
            stackValues.push(std::make_unique<NodeAST>(tokens[i]));
        }
        
        else if (tokens[i].type == Token::Type::Lpar) {
            stackOperants.push(tokens[i]);
        }
        
        else if (tokens[i].type == Token::Type::Rpar) {
            while (!stackOperants.empty() && stackOperants.top().type != Token::Type::Lpar) {
                reduce(stackValues, stackOperants);
            }
            stackOperants.pop();
        }
        
        else {
            while (!stackOperants.empty() && stackOperants.top().type != Token::Type::Lpar && stackOperants.top().precedence >= tokens[i].precedence) {
                reduce(stackValues, stackOperants);
            }
            
            stackOperants.push(tokens[i]);
        }
    }
    while (!stackOperants.empty()) {
        reduce(stackValues, stackOperants);
    }

    return std::move(stackValues.top());
}

// IR


struct IRInstruction {
    enum class OP {
        Const,
        Add,
        Sub,
        Mul,
        Div
    };

    OP operation;
    int dst;
    int src1;
    int src2;
    int val;
};

std::ostream& operator << (std::ostream& cout, IRInstruction& inst)
{
    if (inst.operation == IRInstruction::OP::Const) {
        cout << "r" << inst.dst << " = " << inst.val << std::endl;
    } else {
        std::string op;
        if (inst.operation == IRInstruction::OP::Add) op = "Add";
        else if (inst.operation == IRInstruction::OP::Sub) op = "Sub";
        else if (inst.operation == IRInstruction::OP::Mul) op = "Mul";
        else if (inst.operation == IRInstruction::OP::Div) op = "Div";
        cout << op << " r" << inst.dst << ", r" << inst.src1 << ", r" << inst.src2 << std::endl;
    }
    return cout;
}

struct IRGenerator {
    struct RegisterIndex {
        int curr = 0;

        int getNext() {
            return curr++;
        }
    };

    std::vector<IRInstruction> instructions;
    RegisterIndex index;

    int generate(const std::unique_ptr<NodeAST>& node) {
        IRInstruction newInstruction;
        if (node->token.type == Token::Type::Int) {
            newInstruction.operation = IRInstruction::OP::Const;
            newInstruction.dst = index.getNext();
            newInstruction.val = node->token.val;
            instructions.push_back(newInstruction);
        } else {
            if (node->token.type == Token::Type::Add) newInstruction.operation = IRInstruction::OP::Add;
            else if (node->token.type == Token::Type::Sub) newInstruction.operation = IRInstruction::OP::Sub;
            else if (node->token.type == Token::Type::Mul) newInstruction.operation = IRInstruction::OP::Mul;
            else if (node->token.type == Token::Type::Div) newInstruction.operation = IRInstruction::OP::Div;
            newInstruction.src1 = generate(node->left);
            newInstruction.src2 = generate(node->right);
            newInstruction.dst = index.getNext();
            instructions.push_back(newInstruction);
        }
        return newInstruction.dst;
    }

    void print() {
        for (int i = 0; i < instructions.size(); i++) {
            std::cout << instructions[i];
        }
    }
};

struct VM {
    std::vector<int> registers;
    int pc = 0;
    int result;

    void resizeReg(int dst) {
        if (dst >= registers.size()) {
            registers.resize(dst + 1);
        }
    }

    void run(std::vector<IRInstruction> instructions) {
        while (pc < instructions.size()) {
            switch (instructions[pc].operation)
            {
            case IRInstruction::OP::Const:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = instructions[pc].val;
                pc++;
                break;
            case IRInstruction::OP::Add:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = registers[instructions[pc].src1] + registers[instructions[pc].src2];
                pc++;
                break;
            case IRInstruction::OP::Sub:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = registers[instructions[pc].src1] - registers[instructions[pc].src2];
                pc++;
                break;
            case IRInstruction::OP::Mul:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = registers[instructions[pc].src1] * registers[instructions[pc].src2];
                pc++;
                break;
            case IRInstruction::OP::Div:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = registers[instructions[pc].src1] / registers[instructions[pc].src2];
                pc++;
                break;
            default:
                std::cout << "error message" << std::endl;
                break;
            }
        }
        result = registers.back();
    }
};


int main() {
    Source source = lex("test.elil");
    std::vector<Token> tokens = parse(source);
    std::unique_ptr<NodeAST> ast = createAST(tokens);

    IRGenerator irgenerator;
    irgenerator.generate(ast);
    irgenerator.print();

    VM vm;
    vm.run(irgenerator.instructions);
    std::cout << vm.result << std::endl;

    return 0;
}