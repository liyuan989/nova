#ifndef __NOVA_TOKEN_H__
#define __NOVA_TOKEN_H__

#include <string>
#include <fstream>
#include <unordered_map>
#include <memory>

namespace nova {

enum class TokenType { 
    kIdentifier,
    kKeyword,
    kOperator,
    kNumber,
    kDelimiter,
    kEndOfFile,
    kUnknownType,
};

enum class TokenValue { 
    kIf,
    kThen,
    kElse,
    kEnd,
    kRepeat,
    kUntil,
    kRead,
    kWrite,

    // symbols
    kPlus,              // +
    kMinus,             // -
    kMultiply,          // *
    kDivide,            // /
    kEqual,             // =
    kLess,              // <
    kLeftParenthesis,   // (
    kRightParenthesis,  // )
    kSemicolon,         // ;
    kAssign,            // :=

    kUnReserved,
};

struct TokenRecord {
    TokenRecord(TokenType type, TokenValue value, int precedence)
        : token_type(type), 
          token_value(value), 
          symbol_precedence(precedence) {
    }

    TokenType token_type;
    TokenValue token_value;
    int symbol_precedence;
};

class TokenLocation
{
public:
    TokenLocation()
        : file_name_(""), 
          line_(1), 
          column_(0) { 
    }

    TokenLocation(const std::string& file_name, int token_line, int token_column)
        : file_name_(file_name), 
          line_(token_line), 
          column_(token_column) {
    }

    std::string toString() const {
        return file_name_ + ":" + std::to_string(line_) + ":" + std::to_string(column_) + ":";   
    }

    int line() const { return line_; }
    int column() const { return column_; }
    const std::string& filename() const { return file_name_; }

private:
    std::string file_name_;
    int line_;
    int column_;
};

class Token {
public:
    Token(TokenType token_type, 
          TokenValue token_value, 
          TokenLocation token_location,
          int symbol_precedence,
          const std::string& name);

    Token(TokenType token_type, 
          TokenValue token_value, 
          TokenLocation token_location,
          int symbol_precedence,
          const std::string& name,
          const std::string& string_value);

    Token(TokenType token_type, 
          TokenValue token_value, 
          TokenLocation token_location,
          int symbol_precedence,
          const std::string& name,
          int64_t int_value);

    std::string getTokenTypeDescription() const;

    TokenType getTokenType() const { return token_type_; }
    TokenValue getTokenValue() const { return token_value_; }
    const TokenLocation& getTokenLocation() const { return token_location_; }
    int getSymbolPrecedence() const { return symbol_precedence_; }
    const std::string& getTokenName() const { return name_; } 
    const std::string& getStringValue() const { return string_value_; }
    int64_t getIntValue() const { return int_value_; }

private:
    TokenType token_type_;
    TokenValue token_value_;
    TokenLocation token_location_;
    int symbol_precedence_;
    std::string name_;

    std::string string_value_;
    int64_t int_value_;
};

typedef std::unique_ptr<TokenRecord> TokenRecordPtr;

inline TokenRecordPtr makeTokenRecord(
        TokenType token_type, 
        TokenValue token_value, 
        int symbol_precedence) {
    return std::make_unique<TokenRecord>(token_type, token_value, symbol_precedence);
}
    
} // namespace nova

#endif
