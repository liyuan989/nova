#ifndef __NOVA_SCANNER_H__
#define __NOVA_SCANNER_H__

#include <fstream>
#include <unordered_map>
#include <memory>

#include "token.h"

namespace nova {

class Scanner {
public:
    enum class State { 
        kNone,
        kStart,
        kNumber,
        kIdentifier,
        kComment,
        kOperator,
        kEndOfFile,
    };

    explicit Scanner(const std::string& file_name);

    Scanner(const Scanner&) = delete;
    Scanner& operator=(const Scanner&) = delete;

    Token getNextToken();
    Token getToken() const { return token_; }
    TokenLocation getTokenLocation() { return location_; }
    bool isFileOpened() const { return input_.is_open(); }

    static void setErrorFlag(bool flag) { error_flag_ = flag; }
    static bool getErrorFlag() { return error_flag_; }

private:
    void addToken(const std::string& name, TokenRecordPtr&& record);
    void getNextChar();
    char peekChar();
    void addToBuffer(char c);
    void popBuffer();
    void skipWhitespace();
    void errorReport(const std::string& message);

    void makeToken(TokenType token_type, 
                   TokenValue token_value, 
                   TokenLocation token_location,
                   int symbol_precedence, 
                   const std::string& name);

    void makeToken(TokenType token_type, 
                   TokenValue token_value, 
                   TokenLocation token_location,
                   int symbol_precedence, 
                   const std::string& name, 
                   const std::string& string_value);

    void makeToken(TokenType token_type, 
                   TokenValue token_value, 
                   TokenLocation token_location,
                   int symbol_precedence, 
                   const std::string& name, 
                   int64_t int_value);

    void handleStartState();
    void handleCommentState();
    void handleNumberState();
    void handleIdentifierState();
    void handleAssignState();
    void handleOperatorState();
    void handEndOfFileState();

    void updateTokenLocation() { 
        location_ = TokenLocation(file_name_, line_, column_); 
    }

private:
    typedef std::unordered_map<std::string, TokenRecordPtr> Map;

    std::string file_name_;
    std::ifstream input_;
    int line_;
    int column_;
    TokenLocation location_;
    char current_char_;
    State state_;
    Token token_;
    Map map_;
    std::string buffer_;

    static bool error_flag_;
};
    
} // namespace nova

#endif
