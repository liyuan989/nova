#include "vm.h"

#include <vector>

namespace nova 
{

namespace vm 
{

bool Scanner::error_flag_ = false;

Scanner::Scanner(const std::string& code)
    : input_(code), 
      state_(State::kNone), 
      token_("", TokenValue::kUnReserved, TokenType::kUnknown)
{
    addToken("HALT", TokenValue::kHalt, TokenType::kInstruction);
    addToken("IN", TokenValue::kIn, TokenType::kInstruction);
    addToken("OUT", TokenValue::kOut, TokenType::kInstruction);
    addToken("ADD", TokenValue::kAdd, TokenType::kInstruction);
    addToken("SUB", TokenValue::kSub, TokenType::kInstruction);
    addToken("MUL", TokenValue::kMul, TokenType::kInstruction);
    addToken("DIV", TokenValue::kDiv, TokenType::kInstruction);

    addToken("LD", TokenValue::kLd, TokenType::kInstruction);
    addToken("LDA", TokenValue::kLda, TokenType::kInstruction);
    addToken("LDC", TokenValue::kLdc, TokenType::kInstruction);
    addToken("ST", TokenValue::kSt, TokenType::kInstruction);
    addToken("JLT", TokenValue::kJlt, TokenType::kInstruction);
    addToken("JLE", TokenValue::kJle, TokenType::kInstruction);
    addToken("JGE", TokenValue::kJge, TokenType::kInstruction);
    addToken("JGT", TokenValue::kJgt, TokenType::kInstruction);
    addToken("JEQ", TokenValue::kJeq, TokenType::kInstruction);
    addToken("JNE", TokenValue::kJne, TokenType::kInstruction);

    addToken("(", TokenValue::kLeftParenthesis, TokenType::kOperator);
    addToken(")", TokenValue::kRightParenthesis, TokenType::kOperator);
    addToken(",", TokenValue::kComma, TokenType::kOperator);
    addToken(":", TokenValue::kColon, TokenType::kOperator);
    addToken("+", TokenValue::kPositive, TokenType::kOperator);
    addToken("-", TokenValue::kNegative, TokenType::kOperator);

    getNextChar();
}

void Scanner::addToken(const std::string& name, TokenValue value, TokenType type)
{
    if (map_.find(name) == map_.end()) 
    {
        map_[name] = std::make_unique<Token>(name, value, type);   
    }
}

void Scanner::makeToken(const std::string& name, TokenValue value, TokenType type)
{
    token_ = Token(name, value, type);
    buffer_.clear();
    state_ = State::kNone;
}

void Scanner::getNextChar()
{
    current_char_ = static_cast<char>(input_.get());
}

char Scanner::peekChar()
{
    return static_cast<const char>(input_.peek());
}

void Scanner::addToBuffer(char c)
{
    buffer_.push_back(c);
}

void Scanner::popBuffer()
{
    buffer_.pop_back();
}

void Scanner::skipWhiteSpace()
{
    while (std::isspace(current_char_)) 
    {
        getNextChar();   
    }
}

Token Scanner::getNextToken()
{
    state_ = State::kStart;

    do 
    {
        switch (state_) 
        {
            case State::kStart:
                handleStartState();            
                break;

            case State::kInstruction:
                handleInstructionState();
                break;

            case State::kNumber:
                handleNumberState();
                break;

            case State::kComment:
                handleCommentState();
                break;

            case State::kEndOfFile:
                handleEndOfFile();
                break;

            default:
                handleOperatorState();
                break;
        }
        
    } while (state_ != State::kNone);

    return token_;
}

void Scanner::handleStartState()
{
    skipWhiteSpace();

    if (input_.eof()) 
    {
        state_ = State::kEndOfFile;   
    }
    else if (std::isalpha(current_char_)) 
    {
        state_ = State::kInstruction;       
    }
    else if (std::isdigit(current_char_)) 
    {
        state_ = State::kNumber;   
    }
    else if (current_char_ == '*') 
    {
        state_ = State::kComment;   
    }
    else
    {
        state_ = State::kOperator;
    }
}

void Scanner::handleEndOfFile()
{
    makeToken("eof", TokenValue::kEndOfFile, TokenType::kEndOfFile);
}

void Scanner::handleInstructionState()
{
    while (std::isalpha(current_char_)) 
    {
        addToBuffer(current_char_);
        getNextChar();
    }
    Map::iterator it = map_.find(buffer_);
    if (it != map_.end()) 
    {
        makeToken(it->second->token_name, it->second->token_value, it->second->token_type);
    }
    else
    {
        errorReport("invalid instruction '" + buffer_ + "'");
        state_ = State::kNone;
        buffer_.clear();
    }
}

void Scanner::handleNumberState()
{
    while (std::isdigit(current_char_)) 
    {
        addToBuffer(current_char_);
        getNextChar();
    }
    makeToken(buffer_, TokenValue::kNumber, TokenType::kNumber);
}

void Scanner::handleCommentState()
{
    while (current_char_ != '\n' && current_char_ != '\r') 
    {
        if (input_.eof()) 
        {
            std::string message = "End of file happened in comment, eol is expected, buf find ";
            errorReport(message + current_char_);
            state_ = State::kNone;
            return;
        }
        getNextChar();   
    }
    if (current_char_ == '\r' && peekChar() == '\n') 
    {
        getNextChar();
    }
    getNextChar();
    state_ = State::kStart;
}

void Scanner::handleOperatorState()
{
    addToBuffer(current_char_);
    Map::iterator it = map_.find(buffer_);
    if (it != map_.end()) 
    {
        makeToken(it->second->token_name, it->second->token_value, it->second->token_type);
        getNextChar();
    }
    else
    {
        errorReport("invalid operator '" + buffer_ + "'");
        state_ = State::kNone;
        buffer_.clear();
    }
}

void Scanner::errorReport(const std::string& message)
{
    std::cerr << "vm Token Error: " << message << std::endl;
    setErrorFlag(true);
}

bool VirtualMachine::error_flag_ = false;

VirtualMachine::VirtualMachine(const std::string& code)
    : scanner_(code)
{
    scanner_.getNextToken();
}

void VirtualMachine::run()
{

}

void VirtualMachine::buildInstructions()
{
    while (!isEndOfFile() && !error_flag_) 
    {
        handleCodeLine();
        /*
        if (!isEndOfFile()) 
        {
            expectToken(TokenValue::kEndOfLine, "eol", true);   
        }
        */
    }
}

bool VirtualMachine::isEndOfFile() const
{
    return scanner_.getToken().token_value == TokenValue::kEndOfFile;
}

void VirtualMachine::handleCodeLine()
{
    if (!expectToken(TokenValue::kNumber, "number", false))
    {
        return;
    }
    int line = std::stoi(scanner_.getToken().token_name);
    scanner_.getNextToken();

    if (!expectToken(TokenValue::kColon, ":", true))
    {
        return;
    }

    if (!expectToken(TokenType::kInstruction, "instruction", false))
    {
        return;
    }
    TokenValue token_value = scanner_.getToken().token_value;
    std::string name = scanner_.getToken().token_name;
    scanner_.getNextToken();

    if (!expectToken(TokenValue::kNumber, "number", false)) 
    {
        return;
    }
    int param1 = std::stoi(scanner_.getToken().token_name);  
    scanner_.getNextToken();

    if (!expectToken(TokenValue::kComma, ",", true)) 
    {
        return;   
    }

    bool positive = true;
    if (scanner_.getToken().token_value == TokenValue::kPositive ||
        scanner_.getToken().token_value == TokenValue::kNegative) 
    {
        positive = (scanner_.getToken().token_value == TokenValue::kPositive) ? true : false;
        scanner_.getNextToken();   
    }

    if (!expectToken(TokenValue::kNumber, "number", false)) 
    {
        return;   
    }
    int param2 = std::stoi(scanner_.getToken().token_name);
    if (!positive) 
    {
        param2 = -param2;   
    }
    scanner_.getNextToken();

    int param3;
    if (validateToken(TokenValue::kLeftParenthesis, true)) 
    {
        if (!expectToken(TokenValue::kNumber, "number", false)) 
        {
            return; 
        }
        param3 = std::stoi(scanner_.getToken().token_name);
        scanner_.getNextToken();
        if (!expectToken(TokenValue::kRightParenthesis, ")", true)) 
        {
            return;   
        }
    }
    else if (validateToken(TokenValue::kComma, true)) 
    {
        if (!expectToken(TokenValue::kNumber, "number", false)) 
        {
            return;   
        }
        param3 = std::stoi(scanner_.getToken().token_name);
        scanner_.getNextToken();
    }
    else
    {
        errorReport("expected ',' or '(', but find " + scanner_.getToken().token_name);
        return;
    }

    if (!error_flag_) 
    {
        instructions_[line] = std::make_unique<Instruction>(line, token_value, param1, param2, param3, name); 
    }
}

bool VirtualMachine::validateToken(TokenType type, bool advance_next_token)
{
    if (scanner_.getToken().token_type != type) 
    {
        return false;   
    }
    if (advance_next_token) 
    {   
        scanner_.getNextToken();
    }
    return true;
}

bool VirtualMachine::validateToken(TokenValue value, bool advance_next_token)
{
    if (scanner_.getToken().token_value != value) 
    {
        return false;   
    }
    if (advance_next_token) 
    {
        scanner_.getNextToken();   
    }
    return true;
}

bool VirtualMachine::expectToken(TokenType type, 
                                 const std::string& name, 
                                 bool advance_next_token)
{
    if (scanner_.getToken().token_type != type) 
    {
        errorReport("Expected '" + name + "', but find " + scanner_.getToken().token_name);
        return false;
    }
    if (advance_next_token) 
    {
        scanner_.getNextToken();   
    }
    return true;
}

bool VirtualMachine::expectToken(TokenValue value, 
                                 const std::string& name, 
                                 bool advance_next_token)
{
    if (scanner_.getToken().token_value != value) 
    {
        errorReport("Expected '" + name + "', but find " + scanner_.getToken().token_name);
        return false;   
    }
    if (advance_next_token) 
    {
        scanner_.getNextToken();   
    }
    return true;
}

void VirtualMachine::errorReport(const std::string& message)
{
    std::cerr << "vm Syntax Error: " << message << std::endl;
    setErrorFlag(true);
}

void VirtualMachine::printInstructions() const
{
    for (auto it = instructions_.begin(); it != instructions_.end(); ++it) 
    {
        std::cout << it->second->line << "\t" << it->second->name << "\t" << it->second->param1 
                  << "\t" << it->second->param2 << "\t" << it->second->param3 << std::endl;
    }
}

} // namespace vm
    
} // namespace nova
