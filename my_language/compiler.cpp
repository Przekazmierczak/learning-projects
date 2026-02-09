#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stack>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

struct Token {
    enum class Type {
        Int,
        Add,
        Sub,
        Mul,
        Div,
        Lpar,
        Rpar,
        Var,
        NewL,
        Ind,
        Ded,
        Block,
        Neg,
        Assign
    };

    Type type;

    int val = 0;
    std::string name = "";

    int line = -1;
    int position = -1;

    Token(Type newType)
        : type(newType) {}

    Token(Type newType, int newLine, int newPosition)
        : type(newType), line(newLine), position(newPosition) {}

    Token(Type newType, int newVal, int newLine, int newPosition)
        : type(newType),  val(newVal), line(newLine), position(newPosition) {}

    Token(Type newType, std::string newName, int newLine, int newPosition)
        : type(newType),  name(newName), line(newLine), position(newPosition) {}
};

std::ostream& operator << (std::ostream& cout, Token& token)
{
    if (token.type == Token::Type::Int) {
        cout << "(Int, " << token.val << ", [" << token.line << ", " << token.position << "])";
    } else if (token.type == Token::Type::Var) {
        cout << "(Var, \"" << token.name << "\", [" << token.line << ", " << token.position << "])";
    } else {
        std::string op;
        if (token.type == Token::Type::Add) op = "(Add";
        else if (token.type == Token::Type::Sub) op = "(Sub";
        else if (token.type == Token::Type::Mul) op = "(Mul";
        else if (token.type == Token::Type::Div) op = "(Div";
        else if (token.type == Token::Type::Lpar) op = "(Lpar";
        else if (token.type == Token::Type::Rpar) op = "(Rpar";
        else if (token.type == Token::Type::NewL) op = "(NewL";
        else if (token.type == Token::Type::Assign) op = "(Assign";
        cout << op << ", [" << token.line << ", " << token.position << "])";
    }
    return cout;
}

struct Lexer {
    std::string code;
    std::vector<Token> tokens;
    std::string line;
    std::string current;

    Lexer(std::string codeURL) : code(codeURL) {
        lex();
    }

    void lex() {
        std::ifstream file(code);

        int currLine = 1;
        while (std::getline(file, line)) {
            int currPosition = 1;
            for (char c : line) {
                if (c == ' ') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, tokens, current);
                } else if (c == '+') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, tokens, current);
                    tokens.push_back(Token(Token::Type::Add ,currLine, currPosition));
                } else if (c == '-') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, tokens, current);
                    tokens.push_back(Token(Token::Type::Sub ,currLine, currPosition));
                } else if (c == '*') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, tokens, current);
                    tokens.push_back(Token(Token::Type::Mul ,currLine, currPosition));
                } else if (c == '/') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, tokens, current);
                    tokens.push_back(Token(Token::Type::Div ,currLine, currPosition));
                } else if (c == '(') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, tokens, current);
                    tokens.push_back(Token(Token::Type::Lpar ,currLine, currPosition));
                } else if (c == ')') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, tokens, current);
                    tokens.push_back(Token(Token::Type::Rpar ,currLine, currPosition));
                } else if (c == '=') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, tokens, current);
                    tokens.push_back(Token(Token::Type::Assign ,currLine, currPosition));
                } else {
                    current += c;
                }
                currPosition++;
            }
    
            if (!current.empty()) pushNonOperand(currLine, currPosition, tokens, current);
            currPosition += current.size();
            current.clear();
            
            tokens.push_back(Token(Token::Type::NewL ,currLine, currPosition));
            currLine++;
        }

        for (int i = 0; i < tokens.size(); i++) {
            std::cout << tokens[i] << ", ";
        }
        std::cout << std::endl;
    }

    void pushNonOperand(int currLine, int currPosition, std::vector<Token>& tokens, std::string& current) {
        currPosition = currPosition - current.size();
        if (std::isdigit(static_cast<unsigned char>(current[0]))) {
            for (char c : current) {
                if (!std::isdigit(static_cast<unsigned char>(c))) {
                    std::string errorMsg = "Incorrect variable name: \"" + current
                        + "\"; Line: " + std::to_string(currLine)
                        + ", Position: " + std::to_string(currPosition);
                    std::cerr << errorMsg << std::endl;
                    throw std::invalid_argument(errorMsg);
                }
            }
            tokens.push_back(Token(Token::Type::Int, std::stoi(current), currLine, currPosition));
        } else {
            tokens.push_back(Token(Token::Type::Var, current, currLine, currPosition));
        }
        current.clear();
    }

};

struct Parser {
    struct NodeAST {
        Token token;
        std::unique_ptr<NodeAST> left;
        std::unique_ptr<NodeAST> right;
        std::vector<std::unique_ptr<NodeAST>> statements;
    
        NodeAST(Token newToken) :
            token(newToken) {}
        NodeAST(Token newToken, std::unique_ptr<NodeAST> newLeft) :
            token(newToken),
            left(std::move(newLeft)) {}
        NodeAST(Token newToken, std::unique_ptr<NodeAST> newLeft, std::unique_ptr<NodeAST> newRight) :
            token(newToken),
            left(std::move(newLeft)),
            right(std::move(newRight)) {}
    };

    Parser(std::vector<Token> newTokens) : tokens(newTokens) {
        createAST();
    }

    std::vector<Token> tokens;
    int index = 0;
    std::unique_ptr<NodeAST> ASTroot;

    void createAST() {
        ASTroot = std::move(createBlock());
    }

    std::unique_ptr<NodeAST> createBlock() {
        Token newBlock = Token(Token::Type::Block);
        auto newBlockAST = std::make_unique<NodeAST>(newBlock);

        while (index < tokens.size()) {
            while (tokens[index].type == Token::Type::NewL) {
                index++;
            }
            if (index < tokens.size()) {
                newBlockAST->statements.push_back(std::move(createStatement()));
            }
        }

        return std::move(newBlockAST);
    }

    std::unique_ptr<NodeAST> createStatement() {
        // auto expr = createExpression();
        // if (index < tokens.size() && tokens[index].type == Token::Type::NewL) {
        //     return expr;
        // } else {
        //     std::string errorMsg = "New line symbol is missing; Line: " + std::to_string(tokens[index].line)
        //         + ", Position :" + std::to_string(tokens[index].position);
        //     std::cerr << errorMsg << std::endl;
        //     throw std::invalid_argument(errorMsg);
        // }
        std::unique_ptr<NodeAST> left = createExpression();
        if (index < tokens.size() && tokens[index].type == Token::Type::Assign) {
            if (left->token.type == Token::Type::Var) {
                Token currToken = tokens[index];
                index++;
                std::unique_ptr<NodeAST> right = createExpression();
                left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
            } else {
                std::string errorMsg = "Incorrect left value; Line:" + std::to_string(tokens[index].line);
                std::cerr << errorMsg << std::endl;
                throw std::invalid_argument(errorMsg);
            }
        }
        return left;
    }

    std::unique_ptr<NodeAST> createExpression() {
        return parseAddSub();
    }
    
    std::unique_ptr<NodeAST> parseAddSub() {
        std::unique_ptr<NodeAST> left = parseMulDiv();
        while (index < tokens.size() && (tokens[index].type == Token::Type::Add || tokens[index].type == Token::Type::Sub)) {
            Token currToken = tokens[index];
            index++;
            std::unique_ptr<NodeAST> right = parseMulDiv();
            left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
        }
        return left;
    }

    std::unique_ptr<NodeAST> parseMulDiv() {
        std::unique_ptr<NodeAST> left = parseNeg();
        while (index < tokens.size() && (tokens[index].type == Token::Type::Mul || tokens[index].type == Token::Type::Div)) {
            Token currToken = tokens[index];
            index++;
            std::unique_ptr<NodeAST> right = parseNeg();
            left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
        }
        return left;
    }

    std::unique_ptr<NodeAST> parseNeg() {
        if (index < tokens.size() && tokens[index].type == Token::Type::Sub) {
            index++;
            std::unique_ptr<NodeAST> left = parseNeg();
            Token negToken = Token(Token::Type::Neg);
            left = std::make_unique<NodeAST>(NodeAST(negToken, std::move(left)));
            return left;
        }
        return parsePrimary();
    }

    std::unique_ptr<NodeAST> parsePrimary() {
        if (index < tokens.size() && (tokens[index].type == Token::Type::Int || tokens[index].type == Token::Type::Var)) {
            auto node = std::make_unique<NodeAST>(NodeAST(tokens[index]));
            index++;
            return node;
        }

        if (index < tokens.size() && tokens[index].type == Token::Type::Lpar) {
            index++; // consume "("
            std::unique_ptr<NodeAST> expression = createExpression();
            if (index >= tokens.size() || tokens[index].type != Token::Type::Rpar) {
                std::string errorMsg = "Right parenthesis is missing; Line: " + std::to_string(tokens[index].line);
                std::cerr << errorMsg << std::endl;
                throw std::invalid_argument(errorMsg);
            }
            index++; // consume ")"
            return expression;
        }

        std::string errorMsg = "Syntax error; Line: "
                    + std::to_string(tokens[index].line)
                    + ", Position: "
                    + std::to_string(tokens[index].position);
        std::cerr << errorMsg << std::endl;
        throw std::invalid_argument(errorMsg);
    }
    
};

// IR
struct IRInstruction {
    enum class OP {
        Const,
        Add,
        Sub,
        Mul,
        Div,
        Neg,
        Assign,
        Load
    };

    OP operation;
    int dst;
    int src1;
    int src2;
    int val;
    std::string name;
};

std::ostream& operator << (std::ostream& cout, IRInstruction& inst)
{
    if (inst.operation == IRInstruction::OP::Const) {
        cout << "r" << inst.dst << " = " << inst.val << std::endl;
    } else if (inst.operation == IRInstruction::OP::Neg) {
        cout << "Neg" << " r" << inst.dst << ", r" << inst.src1 << std::endl;
    } else if (inst.operation == IRInstruction::OP::Assign) {
        cout << "\"" << inst.name << "\" <-> r" << inst.dst << std::endl;
    } else if (inst.operation == IRInstruction::OP::Load) {
        cout << "r" << inst.dst << " = \"" << inst.name << "\"" << std::endl;
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

    IRGenerator(const std::unique_ptr<Parser::NodeAST>& node) {
        generate(node);
    }

    std::vector<IRInstruction> instructions;
    RegisterIndex index;

    int generate(const std::unique_ptr<Parser::NodeAST>& node) {
        IRInstruction newInstruction;
        if (node->token.type == Token::Type::Int) {
            newInstruction.operation = IRInstruction::OP::Const;
            newInstruction.dst = index.getNext();
            newInstruction.val = node->token.val;
            instructions.push_back(newInstruction);
        } else if (node->token.type == Token::Type::Block) {
            for (int i = 0; i < node->statements.size(); i++) {
                generate(node->statements[i]);
            }
            return -1;
        } else if (node->token.type == Token::Type::Neg) {
            newInstruction.operation = IRInstruction::OP::Neg;
            newInstruction.src1 = generate(node->left);
            newInstruction.dst = index.getNext();
            instructions.push_back(newInstruction);
        } else if (node->token.type == Token::Type::Assign) {
            newInstruction.operation = IRInstruction::OP::Assign;
            newInstruction.dst = generate(node->right);
            newInstruction.name = node->left->token.name;
            instructions.push_back(newInstruction);
            return -1;
        } else if (node->token.type == Token::Type::Var) {
            newInstruction.operation = IRInstruction::OP::Load;
            newInstruction.dst = index.getNext();
            newInstruction.name = node->token.name;
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
    std::unordered_map<std::string, int> map;

    int pc = 0;
    int result;

    VM(std::vector<IRInstruction> instructions) {
        run(instructions);
    }

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
            case IRInstruction::OP::Neg:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = -registers[instructions[pc].src1];
                pc++;
                break;
            case IRInstruction::OP::Assign:
                map.insert({instructions[pc].name, instructions[pc].dst});
                pc++;
                break;
            case IRInstruction::OP::Load:
                if (map.find(instructions[pc].name) != map.end()) {
                    resizeReg(instructions[pc].dst);
                    registers[instructions[pc].dst] = registers[map[instructions[pc].name]];
                    pc++;
                } else {
                    std::string errorMsg = "Unknown variable name: \"" + instructions[pc].name + "\"";
                    std::cerr << errorMsg << std::endl;
                    throw std::invalid_argument(errorMsg);
                }
                break;
            default:
                std::cerr << "Unknown opperation" << std::endl;
                throw std::invalid_argument("Unknown opperation");
                break;
            }
        }
    }
};

int main() {
    Lexer lexer("test.elil");

    Parser parser(lexer.tokens);

    IRGenerator irgenerator(parser.ASTroot);
    irgenerator.print();

    VM vm(irgenerator.instructions);

    for (int i = 0; i < vm.registers.size(); i++) {
        std::cout << vm.registers[i] << ", ";
    }
    
    std::cout << std::endl;
    return 0;
}