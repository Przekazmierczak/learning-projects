#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stack>
#include <memory>
#include <iostream>
#include <stdexcept>

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
        Neg
    };

    Type type;
    bool operant = false;
    int precedence = 0;

    int val = 0;

    std::string name = "";

    int line;
    int position;

    static Token createAdd(int line, int position) {
        Token newToken;
        newToken.type = Token::Type::Add;
        newToken.operant = true;
        newToken.precedence = 20;
        newToken.line = line;
        return newToken;
    }

    static Token createSub(int line, int position) {
        Token newToken;
        newToken.type = Token::Type::Sub;
        newToken.operant = true;
        newToken.precedence = 20;
        newToken.line = line;
        return newToken;
    }

    static Token createMul(int line, int position) {
        Token newToken;
        newToken.type = Token::Type::Mul;
        newToken.operant = true;
        newToken.precedence = 30;
        newToken.line = line;
        return newToken;
    }

    static Token createDiv(int line, int position) {
        Token newToken;
        newToken.type = Token::Type::Div;
        newToken.operant = true;
        newToken.precedence = 30;
        newToken.line = line;
        newToken.position = position;
        return newToken;
    }

    static Token createLpar(int line, int position) {
        Token newToken;
        newToken.type = Token::Type::Lpar;
        newToken.operant = true;
        newToken.precedence = 40;
        newToken.line = line;
        newToken.position = position;
        return newToken;
    }

    static Token createRpar(int line, int position) {
        Token newToken;
        newToken.type = Token::Type::Rpar;
        newToken.operant = true;
        newToken.precedence = 10;
        newToken.line = line;
        newToken.position = position;
        return newToken;
    }

    static Token createNewL(int line, int position) {
        Token newToken;
        newToken.type = Token::Type::NewL;
        newToken.line = line;
        newToken.position = position;
        return newToken;
    }
    
    static Token createInt(int line, int position, int val) {
        Token newToken;
        newToken.type = Token::Type::Int;
        newToken.val = val;
        newToken.line = line;
        newToken.position = position;
        return newToken;
    }

    static Token createVar(int line, int position, std::string name) {
        Token newToken;
        newToken.type = Token::Type::Var;
        newToken.name = name;
        newToken.line = line;
        newToken.position = position;
        return newToken;
    }
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
                    if (!current.empty()) pushNonOperant(currLine, currPosition, tokens, current);
                } else if (c == '+') {
                    if (!current.empty()) pushNonOperant(currLine, currPosition, tokens, current);
                    tokens.push_back(Token::createAdd(currLine, currPosition));
                } else if (c == '-') {
                    if (!current.empty()) pushNonOperant(currLine, currPosition, tokens, current);
                    tokens.push_back(Token::createSub(currLine, currPosition));
                } else if (c == '*') {
                    if (!current.empty()) pushNonOperant(currLine, currPosition, tokens, current);
                    tokens.push_back(Token::createMul(currLine, currPosition));
                } else if (c == '/') {
                    if (!current.empty()) pushNonOperant(currLine, currPosition, tokens, current);
                    tokens.push_back(Token::createDiv(currLine, currPosition));
                } else if (c == '(') {
                    if (!current.empty()) pushNonOperant(currLine, currPosition, tokens, current);
                    tokens.push_back(Token::createLpar(currLine, currPosition));
                } else if (c == ')') {
                    if (!current.empty()) pushNonOperant(currLine, currPosition, tokens, current);
                    tokens.push_back(Token::createRpar(currLine, currPosition));
                } else {
                    current += c;
                }
            }
    
            if (!current.empty()) pushNonOperant(currLine, currPosition, tokens, current);
            currPosition += current.size();
            current.clear();
            
            tokens.push_back(Token::createNewL(currLine, currPosition));
            currLine++;
        }

        for (int i = 0; i < tokens.size(); i++) {
            std::cout << tokens[i] << ", ";
        }
        std::cout << std::endl;
    }

    void pushNonOperant(int currLine, int currPosition, std::vector<Token>& tokens, std::string& current) {
        if (std::isdigit(current[0])) {
            tokens.push_back(Token::createInt(currLine, currPosition, std::stoi(current)));
        } else {
            tokens.push_back(Token::createVar(currLine, currPosition, current));
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
        Token newBlock;
        newBlock.type = Token::Type::Block;
        auto newBlockAST = std::make_unique<NodeAST>(newBlock);

        while (index < tokens.size()) {
            if (tokens[index].type == Token::Type::NewL) {
                index++;
            }
            if (index < tokens.size()) {
                newBlockAST->statements.push_back(std::move(createStatement()));
            }
        }

        return std::move(newBlockAST);
    }

    std::unique_ptr<NodeAST> createStatement() {
        return createExpression();
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
            Token negToken;
            negToken.type = Token::Type::Neg;
            left = std::make_unique<NodeAST>(NodeAST(negToken, std::move(left)));
            return left;
        }
        return parsePrimary();
    }

    std::unique_ptr<NodeAST> parsePrimary() {
        if (index < tokens.size() && (tokens[index].type == Token::Type::Int) || tokens[index].type == Token::Type::Var) {
            auto node = std::make_unique<NodeAST>(NodeAST(tokens[index]));
            index++;
            return node;
        }

        if (index < tokens.size() && tokens[index].type == Token::Type::Lpar) {
            index++; // consume "("
            std::unique_ptr<NodeAST> expression = createExpression();
            if (index >= tokens.size() || tokens[index].type != Token::Type::Rpar) {
                std::cout << "Right parenthesis is missing" << std::endl;
                throw std::invalid_argument( "Right parenthesis is missing" );
            }
            index++; // consume ")"
            return expression;
        }

        std::cout << "Unknown primary" << std::endl;
        throw std::invalid_argument( "Unknown primary" );
        return nullptr;
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
        Neg
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
    } else if (inst.operation == IRInstruction::OP::Neg) {
        cout << "Neg" << " r" << inst.dst << ", r" << inst.src1 << std::endl;
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
            default:
                std::cout << "Unknown opperant" << std::endl;
                throw std::invalid_argument( "Unknown opperant" );
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