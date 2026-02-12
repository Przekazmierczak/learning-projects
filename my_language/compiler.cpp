#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <regex>

[[noreturn]] void throwError(std::string errorMsg) {
    std::cerr << errorMsg << std::endl;
    throw std::invalid_argument(errorMsg);
}

[[noreturn]] void throwError(std::string errorMsg, int line) {
    std::string newErrorMsg = errorMsg + "; Line: " + std::to_string(line);
    std::cerr << newErrorMsg << std::endl;
    throw std::invalid_argument(newErrorMsg);
}

[[noreturn]] void throwError(std::string errorMsg, int line, int position) {
    std::string newErrorMsg = errorMsg + "; Line: " + std::to_string(line) + ", Position: " + std::to_string(position);
    std::cerr << newErrorMsg << std::endl;
    throw std::invalid_argument(newErrorMsg);
}

struct Token {
    enum class Type {
        Int,
        Add, Sub, Mul, Div,
        Lpar, Rpar,
        Var,
        NewL,
        Ind, Ded,
        Block,
        Neg,
        Assign,
        If, Else,
        Scolon
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

static const char* names[] = {
    "Int",
    "Add", "Sub", "Mul", "Div",
    "Lpar", "Rpar",
    "Var",
    "NewL",
    "Ind", "Ded",
    "Block",
    "Neg",
    "Assign",
    "If", "Else",
    "Scolon",
};

std::ostream& operator << (std::ostream& cout, const Token& token)
{
    if (token.type == Token::Type::Int) {
        cout << "(" << names[static_cast<int>(token.type)] << ", " << token.val << ", [" << token.line << ", " << token.position << "])";
    } else if (token.type == Token::Type::Var) {
        cout << "(" << names[static_cast<int>(token.type)] << ", \"" << token.name << "\", [" << token.line << ", " << token.position << "])";
    } else {
        cout << "(" << names[static_cast<int>(token.type)] << ", [" << token.line << ", " << token.position << "])";
    }
    return cout;
}

struct Lexer {
    std::string code;
    std::vector<Token> tokens;
    std::regex patternInt{R"(^\d+$)"};
    std::regex patternName{R"(^[a-zA-Z_][a-zA-Z0-9_]*$)"};

    Lexer(std::string codeURL) : code(codeURL) {
        lex();
    }

    void lex() {
        std::ifstream file(code);
        std::string current;
        std::string line;
        std::vector<int> indents;

        indents.push_back(0);
        int currLine = 1;
        while (std::getline(file, line)) {
            int currPosition = 1;
            int index = 0;

            // Check current indentation
            while (index < line.size() && line[index] == ' ') {
                index++;
            };

            // Ignore empty line
            if (index >= line.size()) continue;

            // Add Ind, Ded if indantation changed
            if (indents.empty() || index > indents.back()) {
                indents.push_back(index);
                tokens.push_back(Token(Token::Type::Ind, currLine, 1));
            } else if (index < indents.back()) {
                while (index != indents.back()) {
                    indents.pop_back();
                    tokens.push_back(Token(Token::Type::Ded, currLine, 1));
                    if (indents.empty()) {
                        throwError("Incorrect indentation", currLine);
                    }
                }
            }
            currPosition += index;
            
            for (; index < line.size(); index++) {
                if (line[index] == ' ') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                } else if (line[index] == '+') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Add ,currLine, currPosition));
                } else if (line[index] == '-') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Sub ,currLine, currPosition));
                } else if (line[index] == '*') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Mul ,currLine, currPosition));
                } else if (line[index] == '/') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Div ,currLine, currPosition));
                } else if (line[index] == '(') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Lpar ,currLine, currPosition));
                } else if (line[index] == ')') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Rpar ,currLine, currPosition));
                } else if (line[index] == '=') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Assign ,currLine, currPosition));
                } else if (line[index] == ';') {
                    if (!current.empty()) pushNonOperand(currLine, currPosition, current);
                    tokens.push_back(Token(Token::Type::Scolon ,currLine, currPosition));
                } else {
                    current += line[index];
                }
                currPosition++;
            }
    
            if (!current.empty()) pushNonOperand(currLine, currPosition, current);
            currPosition += current.size();
            current.clear();
            
            tokens.push_back(Token(Token::Type::NewL ,currLine, currPosition));
            currLine++;
        }

        // Add missing Ded to close indentation
        while (indents.size() > 1) {
            indents.pop_back();
            tokens.push_back(Token(Token::Type::Ded, currLine, 1));
        }
    }

    void pushNonOperand(int currLine, int currPosition, std::string& current) {
        currPosition = currPosition - current.size();
        if (current == "if") {
            tokens.push_back(Token(Token::Type::If, currLine, currPosition));
        } else if (current == "else") {
            tokens.push_back(Token(Token::Type::Else, currLine, currPosition));
        } else if (std::regex_match(current, patternInt)) {
            tokens.push_back(Token(Token::Type::Int, std::stoi(current), currLine, currPosition));
        } else if (std::regex_match(current, patternName)) {
            tokens.push_back(Token(Token::Type::Var, current, currLine, currPosition));
        } else {
            throwError("Incorrect name \"" + current + "\"", currLine, currPosition);
        }
        current.clear();
    }

    void printTokens() {
        for (int i = 0; i < tokens.size(); i++) {
            std::cout << tokens[i] << ", ";
        }
        std::cout << std::endl;
    }

};

struct Parser {
    struct NodeAST {
        Token token;
        std::unique_ptr<NodeAST> left = nullptr;
        std::unique_ptr<NodeAST> right = nullptr;
        std::vector<std::unique_ptr<NodeAST>> statements;
        std::unique_ptr<NodeAST> condition = nullptr;
    
        NodeAST(Token newToken) :
            token(newToken) {}
        NodeAST(Token newToken, std::unique_ptr<NodeAST> newLeft) :
            token(newToken),
            left(std::move(newLeft)) {}
        NodeAST(Token newToken, std::unique_ptr<NodeAST> newLeft, std::unique_ptr<NodeAST> newRight) :
            token(newToken),
            left(std::move(newLeft)),
            right(std::move(newRight)) {}
        NodeAST(Token newToken, std::unique_ptr<NodeAST> newCondition, std::unique_ptr<NodeAST> newLeft, std::unique_ptr<NodeAST> newRight) :
            token(newToken),
            condition(std::move(newCondition)),
            left(std::move(newLeft)),
            right(std::move(newRight)) {}
    };

    std::vector<Token> tokens;
    int index = 0;
    std::unique_ptr<NodeAST> ASTroot;

    Parser(const std::vector<Token>& newTokens) : tokens(newTokens) {
        createAST();
    }

    void createAST() {
        ASTroot = std::move(createBlock());
    }

    std::unique_ptr<NodeAST> createBlock() {
        Token newBlock = Token(Token::Type::Block);
        auto newBlockAST = std::make_unique<NodeAST>(newBlock);

        while (index < tokens.size()) {
            while (index < tokens.size() && tokens[index].type == Token::Type::NewL) {
                index++;
            }

            if (index >= tokens.size()) break;

            if (tokens[index].type == Token::Type::Ded) {
                index++; // consume Ded
                break;
            } else {
                newBlockAST->statements.push_back(std::move(createStatement()));
            }
        }

        return std::move(newBlockAST);
    }

    std::unique_ptr<NodeAST> createStatement() {
        if (index < tokens.size() && tokens[index].type == Token::Type::If) {
            return createIfStatement();
        }

        std::unique_ptr<NodeAST> left = createExpression();
        if (index < tokens.size() && tokens[index].type == Token::Type::Assign) {
            if (left->token.type == Token::Type::Var) {
                Token currToken = tokens[index];
                index++;
                std::unique_ptr<NodeAST> right = createExpression();
                left = std::make_unique<NodeAST>(NodeAST(currToken, std::move(left), std::move(right)));
            } else {
                throwError("Incorrect left value", tokens[index].line);
            }
        } else if (index < tokens.size() && tokens[index].type == Token::Type::NewL) {
    
        } else {
            throwError("Incorrect symbol after expression", tokens[index].line, tokens[index].position);
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
            Token negToken = Token(Token::Type::Neg, tokens[index].line, tokens[index].position);
            index++;
            std::unique_ptr<NodeAST> left = parseNeg();
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
                throwError("Right parenthesis is missing", tokens[index].line, tokens[index].position);
            }
            index++; // consume ")"
            return expression;
        }
        throwError("Syntax error", tokens[index].line, tokens[index].position);
    }

    std::unique_ptr<NodeAST> createIfStatement() {
        Token ifToken = tokens[index];
        index++; // consume if
        std::unique_ptr<NodeAST> condition = createExpression();
        consumeSNI();
        std::unique_ptr<NodeAST> ifBlock = createBlock();

        std::unique_ptr<NodeAST> elseBlock = nullptr;
        if (index < tokens.size() && tokens[index].type == Token::Type::Else) {
            index++; // consume else
            consumeSNI();
            elseBlock = createBlock();
        }

        return std::make_unique<NodeAST>(NodeAST(ifToken, std::move(condition), std::move(ifBlock), std::move(elseBlock)));
    }

    // Check and consume Semicolon, New line and Indentation tokens
    void consumeSNI() {
        if (index >= tokens.size() || tokens[index].type != Token::Type::Scolon) {
            throwError("\";\" is missing after if statemant", tokens[index].line, tokens[index].position);
        }
        index++; // consume ";"

        if (index >= tokens.size() || tokens[index].type != Token::Type::NewL) {
            throwError("New line is missing after if statemant", tokens[index].line, tokens[index].position);
        }
        index++; // consume NewL

        if (index >= tokens.size() || tokens[index].type != Token::Type::Ind) {
            throwError("Indentation is missing after if statemant", tokens[index].line, tokens[index].position);
        }
        index++; // consume Ind
    }
};

// IR
struct IRInstruction {
    enum class OP {
        Const,
        Add, Sub, Mul, Div,
        Neg,
        Assign, Load,
        Cmp,
        Jmp, JmpZ, JmpNZ
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
        cout << "Move r" << inst.dst << ", " << inst.val << std::endl;
    } else if (inst.operation == IRInstruction::OP::Neg) {
        cout << "Neg" << " r" << inst.dst << ", r" << inst.src1 << std::endl;
    } else if (inst.operation == IRInstruction::OP::Assign) {
        cout << "Assign \"" << inst.name << "\", r" << inst.dst << std::endl;
    } else if (inst.operation == IRInstruction::OP::Load) {
        cout << "Load r" << inst.dst << ", \"" << inst.name << "\"" << std::endl;
    } else if (inst.operation == IRInstruction::OP::Jmp) {
        cout << "Jmp pc" << inst.dst << std::endl;
    } else if (inst.operation == IRInstruction::OP::JmpZ) {
        cout << "JmpZ pc" << inst.dst << ", r" << inst.src1 << std::endl;
    } else {
        std::string op;
        if (inst.operation == IRInstruction::OP::Add) op = "Add";
        else if (inst.operation == IRInstruction::OP::Sub) op = "Sub";
        else if (inst.operation == IRInstruction::OP::Mul) op = "Mul";
        else if (inst.operation == IRInstruction::OP::Div) op = "Div";
        else if (inst.operation == IRInstruction::OP::Cmp) op = "Cmp";
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
            return instructions.size();
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
        } else if (node->token.type == Token::Type::If) {
            addIfInstructions(node);
            return -1;
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

    void addIfInstructions(const std::unique_ptr<Parser::NodeAST>& node) {
        IRInstruction cmp;
        cmp.operation = IRInstruction::OP::Cmp;
        cmp.src1 = generate(node->condition);
        cmp.src2 = addConst(0);;
        cmp.dst = index.getNext();
        instructions.push_back(cmp);

        IRInstruction JmpZ;
        JmpZ.operation = IRInstruction::OP::JmpZ;
        JmpZ.src1 = cmp.dst;
        
        int pc = instructions.size(); // Save Jmpz location
        instructions.push_back(JmpZ);
        instructions[pc].dst = generate(node->left);

        if (node->right) {
            instructions[pc].dst++; // Pass over new Jmp instruction

            IRInstruction Jmp;
            Jmp.operation = IRInstruction::OP::Jmp;
            pc = instructions.size();
            instructions.push_back(Jmp);
            instructions[pc].dst = generate(node->right);
        }
    }

    int addConst(int val) {
        IRInstruction mov;
        mov.operation = IRInstruction::OP::Const;
        mov.dst = index.getNext();
        mov.val = val;
        instructions.push_back(mov);
        return mov.dst;
    }

    void print() {
        for (int i = 0; i < instructions.size(); i++) {
            std::cout << i << ". " << instructions[i];
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

    void run(const std::vector<IRInstruction>& instructions) {
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
                if (registers[instructions[pc].src2] == 0) throwError("Divide by zero error");
                registers[instructions[pc].dst] = registers[instructions[pc].src1] / registers[instructions[pc].src2];
                pc++;
                break;
            case IRInstruction::OP::Neg:
                resizeReg(instructions[pc].dst);
                registers[instructions[pc].dst] = -registers[instructions[pc].src1];
                pc++;
                break;
            case IRInstruction::OP::Assign:
                map[instructions[pc].name] = instructions[pc].dst;
                pc++;
                break;
            case IRInstruction::OP::Load:
                if (map.find(instructions[pc].name) != map.end()) {
                    resizeReg(instructions[pc].dst);
                    registers[instructions[pc].dst] = registers[map[instructions[pc].name]];
                    pc++;
                } else {
                    throwError("Unknown variable name: \"" + instructions[pc].name + "\"");
                }
                break;
            case IRInstruction::OP::Cmp:
                registers[instructions[pc].dst] = registers[instructions[pc].src1] - registers[instructions[pc].src2];
                pc++;
                break;
            case IRInstruction::OP::Jmp:
                pc = instructions[pc].dst;
                break;
            case IRInstruction::OP::JmpZ:
                if (registers[instructions[pc].src1] != 0) {
                    pc++;
                } else {
                    pc = instructions[pc].dst;
                }
                break;
            default:
                throwError("Unknown operation");
                break;
            }
        }
    }
};

int main() {
    Lexer lexer("test.elil");
    lexer.printTokens();

    Parser parser(lexer.tokens);

    IRGenerator irgenerator(parser.ASTroot);
    irgenerator.print();

    VM vm(irgenerator.instructions);

    for (int i = 0; i < vm.registers.size(); i++) {
        std::cout << "(r" << i << "=" << vm.registers[i] << "),";
    }
    
    std::cout << std::endl;
    return 0;
}