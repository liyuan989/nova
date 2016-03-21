#ifndef __NOVA_VM_H__
#define __NOVA_VM_H__

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <memory>

namespace nova 
{

namespace vm 
{

enum class TokenType 
{ 
    kNumber,
    kInstruction, 
    kOperator,
    kEndOfFile,
    kUnknown,
};

enum class TokenValue 
{
    // operator
    kLeftParenthesis,    // (
    kRightParenthesis,   // )
    kComma,              // ,
    kColon,              // :
    kPositive,           // +
    kNegative,           // -

    kNumber,

    kEndOfFile,

    // RO  opcode r,s,t
    kHalt,
    kIn,
    kOut,
    kAdd,
    kSub,
    kMul,
    kDiv,

    // RM  opcode r,d(s)
    kLd,
    kLda,
    kLdc,
    kSt,
    kJlt,
    kJle,
    kJge,
    kJgt,
    kJeq,
    kJne,

    kUnReserved,
};

struct Token
{
    Token(const std::string& name, TokenValue value, TokenType type)
        : token_name(name), token_value(value), token_type(type)
    {
    }

    std::string token_name;
    TokenValue token_value;
    TokenType token_type;
};

typedef std::unique_ptr<Token> TokenPtr;

class Scanner
{
public:
    enum class State 
    { 
        kNone,
        kStart,
        kInstruction,
        kNumber,
        kComment,
        kOperator,
        kEndOfFile,
        kEndOfLine,
    };

    explicit Scanner(const std::string& code);
    Scanner(const Scanner&) = delete;
    Scanner& operator=(const Scanner&) = delete;

    Token getNextToken();

    Token getToken() const
    {
        return token_;
    }

    static bool getErrorFlag()
    {
        return error_flag_;
    }

    static void setErrorFlag(bool flag)
    {
        error_flag_ = flag;
    }

private:
    void addToken(const std::string& name, TokenValue value, TokenType type);
    void makeToken(const std::string& name, TokenValue value, TokenType type);
    void getNextChar();
    char peekChar();
 
    void addToBuffer(char c);
    void popBuffer();
    void skipWhiteSpace();

    void handleStartState();
    void handleInstructionState();
    void handleNumberState();
    void handleCommentState();
    void handleOperatorState();
    void handleEndOfFile();

    void errorReport(const std::string& message);

private:
    typedef std::unordered_map<std::string, TokenPtr> Map;

    Map map_;
    std::istringstream input_;
    std::string buffer_;
    State state_;
    Token token_;
    char current_char_;

    static bool error_flag_;
};
 
struct Instruction 
{
    Instruction(int l, TokenValue value, int p1, int p2, int p3, const std::string& n)
        : line(l),
          token_value(value),
          param1(p1),
          param2(p2),
          param3(p3),
          name(n)
    {
    }

    int line;
    TokenValue token_value;
    int param1;
    int param2;
    int param3;
    std::string name;
};

typedef std::unique_ptr<Instruction> InstructionPtr;

class VirtualMachine
{
public:
    static const int kRegisterCount = 8;
    static const int kPc = 7;
    static const int kMp = 6;

    explicit VirtualMachine(const std::string& code);
    VirtualMachine(const VirtualMachine&) = delete;
    VirtualMachine& operator=(const VirtualMachine&) = delete;

    void buildInstructions();
    void run();

    void printInstructions() const;  // for debug

    static bool getErrorFlag()
    {
        return error_flag_;
    }

    static void setErrorFlag(bool flag)
    {
        error_flag_ = flag;
    }

private:
    bool isEndOfFile() const;
    void handleCodeLine();
    
    bool validateToken(TokenType type, bool advance_next_token);
    bool validateToken(TokenValue value, bool advance_next_token);
    bool expectToken(TokenType type, const std::string& name, bool advance_next_token);
    bool expectToken(TokenValue value, const std::string& name, bool advance_next_token);
    void errorReport(const std::string& message);

    bool checkRegisterNumber(int num);
    void pushMemory(int index, int val, bool tmp_mem);
    int loadMemory(int index, bool tmp_mem);

private:
    typedef std::map<int, InstructionPtr> InstructionList;

    Scanner scanner_;
    InstructionList instructions_;
    int registers_[kRegisterCount];
    std::vector<int> global_mem_;
    std::vector<int> tmp_mem_;

    static bool error_flag_;
};
   
} // namespace vm
    
} // namespace nova

#endif
